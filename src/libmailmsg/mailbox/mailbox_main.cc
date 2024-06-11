/* mailbox_main SUPPORT */
/* lang=C++20 */

/* mailbox handling object */
/* version %I% last-modified %G% */

#define	CF_READTO	0		/* use a read timeout? */

/* revision history:

	= 2009-01-10, David A­D­ Morano
        This is being written from scratch to finally get an abstracted
        "mailbox" that is fast enough for interactive use.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is a *simple* mailbox object.  It reads a mailbox-formatted
	file and allows some simple manipulations of it.

	Notes on message status:

	-	new
	O	old and unread
	R	read but not old
	RO	read previously (old and read)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* |INT_MAX| */
#include	<csignal>
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<sigblocker.h>
#include	<mallocxx.h>
#include	<filer.h>
#include	<fbliner.h>
#include	<ascii.h>
#include	<vecobj.h>
#include	<vecstr.h>
#include	<strn.h>
#include	<mkpathx.h>
#include	<sncpyx.h>
#include	<matxstr.h>
#include	<lockfile.h>
#include	<intceil.h>
#include	<hdrextnum.h>
#include	<opentmp.h>
#include	<char.h>
#include	<hasx.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"mailbox.h"
#include	"mailmsghdrval.h"
#include	"mailmsghdrfold.h"
#include	"mailmsgmatenv.h"
#include	"mailmsgmathdr.h"


/* local defines */

#ifndef	MSGCOLS
#define	MSGCOLS			76
#endif

#define	MAILBOX_DEFMSGS		100
#define	MAILBOX_READSIZE	(4*1024)
#define	MAILBOX_TMPVARDNAME	"/var/tmp"
#define	MAILBOX_SUBDNAME	"mailboxes"

#define	MB_MI			mailbox_mi

#define	FMAT(cp)	((cp)[0] == 'F')

#define	TO_LOCK		120		/* mailbox lock-timeout */
#define	TO_READ		5

#define	MSGHEADCONT(ch)	(((ch) == CH_SP) || ((ch) == CH_TAB))

#define	TMPDIRMODE	(0777 | S_ISVTX)

#ifndef	SIGACTION
#define	SIGACTION	struct sigaction
#endif

#define	MSGCOPY		struct msgcopy

#define	SIGSTATE	struct sigstate

#define	MAILBOXPI	struct mailboxpi
#define	MAILBOXPI_FL	struct mailboxpi_flags

#define	MMHV		mailmsghdrval

#ifndef	CF_READTO
#define	CF_READTO	0		/* use a read timeout? */
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	tmpmailboxes(char *,int) noex ;
}


/* external variables */


/* local structures */

struct sigstate {
	SIGACTION	*sao ;
	sigset_t	oldsigmask ;
} ;

struct msgcopy {
	filer		*fbp ;
	char		*bp ;
	off_t		moff ;
	int		bl ;
} ;

struct mailboxpi_flags {
	uint		fmsg ;
	uint		fenv ;
	uint		fhdr ;
	uint		fmhv ;
	uint		feoh ;
	uint		fbol ;
	uint		feol ;
} ;

struct mailboxpi {
	fbliner		*lsp ;
	MAILBOXPI_FL	f ;
	int		mi ;
} ;


/* forward references */

template<typename ... Args>
static int mailbox_ctor(mailbox *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    memclear(op) ;		/* dangerous */
	    if ((op->mlp = new(nothrow) vecobj) != nullptr) {
		rs = SR_OK ;
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailbox_ctor) */

static int mailbox_dtor(mailbox *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->mlp) {
		delete op->mlp ;
		op->mlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailbox_dtor) */

static int mailbox_opener(mailbox *,cc *,int) noex ;
static int mailbox_parse(mailbox *) noex ;
static int mailbox_parsemsg(mailbox *,fbliner *,int) noex ;
static int mailbox_parsemsger(mailbox *,mmenvdat *,MAILBOXPI *) noex ;
static int mailbox_loadmsghead(mailbox *,MB_MI *,mailmsghdrval *) noex ;
static int mailbox_msgfins(mailbox *) noex ;
static int mailbox_rewrite(mailbox *) noex ;
static int mailbox_rewrites(mailbox *,cchar *) noex ;
static int mailbox_rewriter(mailbox *,int) noex ;
static int mailbox_msgcopy(mailbox *,MSGCOPY *,MB_MI *) noex ;
static int mailbox_msgcopyadd(mailbox *,MSGCOPY *,MB_MI *) noex ;

static int msginfo_start(MB_MI *,off_t,int) noex ;
static int msginfo_finish(MB_MI *) noex ;
static int msginfo_setenv(MB_MI *,mmenvdat *) noex ;

static int mailboxpi_start(MAILBOXPI *,fbliner *,int) noex ;
static int mailboxpi_finish(MAILBOXPI *) noex ;
static int mailboxpi_havemsg(MAILBOXPI *) noex ;

static int writeblanks(int,int) noex ;


/* local variables */

enum headkeys {
	headkey_clen,
	headkey_clines,
	headkey_lines,
	headkey_xlines,
	headkey_overlast
} ;

static constexpr cpcchar	msghdrkeys[] = {
	"content-length",
	"content-lines",
	"lines",
	"x-lines",
	nullptr
} ;

static constexpr int	sigblockers[] = {
	SIGUSR1,
	SIGUSR2,
	SIGQUIT,
	SIGCHLD,
	SIGTERM,
	SIGINT,
	SIGPOLL,
	0
} ;

constexpr bool		f_readto = CF_READTO ;


/* exported variables */


/* exported subroutines */

int mailbox_open(mailbox *op,cchar *mbfname,int mflags) noex {
	int		rs ;
	int		nmsgs = 0 ;
	if ((rs = mailbox_ctor(op,mbfname)) >= 0) {
	    rs = SR_INVALID ;
	    if (mbfname[0]) {
	        int	oflags = 0 ;
	        op->mfd = -1 ;
	        op->msgs_total = 0 ;
	        op->to_lock = TO_LOCK ;
	        op->to_read = TO_READ ;
	        op->mflags = mflags ;
	        op->pagesize = getpagesize() ;
	        op->f.readonly = (! (mflags & MAILBOX_ORDWR)) ;
	        op->f.useclen = (! (mflags & MAILBOX_ONOCLEN)) ;
	        op->f.useclines = MKBOOL(mflags & MAILBOX_OUSECLINES) ;
	        oflags |= (mflags & MAILBOX_ORDWR) ? O_RDWR : 0 ;
		rs = mailbox_opener(op,mbfname,oflags) ;
		nmsgs = rs ;
	    } /* end if (valid) */
	    if (rs < 0) {
		mailbox_dtor(op) ;
	    }
	} /* end if (mailbox_ctor) */
	return (rs >= 0) ? nmsgs : rs ;
}
/* end subroutine (mailbox_open) */

int mailbox_close(mailbox *op) noex {
	int		rs ;
	int		rs1 ;
	int		mblen = 0 ;
	if ((rs = mailbox_magic(op)) >= 0) {
	    bool	f = true ;
	    f = f && (! op->f.readonly) ;
	    f = f && ((op->msgs_del > 0) || op->f.rewrite) ;
	    if (f) {
	        rs1 = mailbox_rewrite(op) ;
	        if (rs >= 0) rs = rs1 ;
	        mblen = rs ;
	    }
	    {
	        rs1 = mailbox_msgfins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = vecobj_finish(op->mlp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->mailfname) {
	        rs1 = uc_free(op->mailfname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->mailfname = nullptr ;
	    }
	    if (op->mfd >= 0) {
	        rs1 = u_close(op->mfd) ;
	        if (rs >= 0) rs = rs1 ;
	        op->mfd = -1 ;
	    }
	    {
	        rs1 = mailbox_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return (rs >= 0) ? mblen : rs ;
}
/* end subroutine (mailbox_close) */

int mailbox_mbfile(mailbox *op,char *mbuf,int mlen) noex {
	int		rs ;
	if ((rs = mailbox_magic(op,mbuf)) >= 0) {
	    rs = sncpy1(mbuf,mlen,op->mailfname) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailbox_mbfile) */

int mailbox_getinfo(mailbox *op,mailbox_info *mip) noex {
	int		rs ;
	int		total = 0 ;
	if ((rs = mailbox_magic(op,mip)) >= 0) {
	    memclear(mip) ;
	    mip->nmsgs = op->msgs_total ;
	    total = op->msgs_total ;
	} /* end if (non-null) */
	return (rs >= 0) ? total : rs ;
}
/* end subroutine (mailbox_getinfo) */

int mailbox_count(mailbox *op) noex {
	int		rs ;
	int		total = 0 ;
	if ((rs = mailbox_magic(op)) >= 0) {
	    total = op->msgs_total ;
	} /* end if (magic) */
	return (rs >= 0) ? total : rs ;
}
/* end subroutine (mailbox_count) */

int mailbox_check(mailbox *op,time_t dt) noex {
	int		rs ;
	int		f = false ;
	if ((rs = mailbox_magic(op)) >= 0) {
	    if (dt == 0) dt = time(nullptr) ;
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mailbox_check) */

int mailbox_msgdel(mailbox *op,int mi,int f) noex {
	int		rs ;
	int		f_prev = false ;
	if ((rs = mailbox_magic(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (mi >= 0) {
	        rs = SR_NOTFOUND ;
	        if (mi < op->msgs_total) {
	    	    rs = SR_ROFS ;
	            if (! op->f.readonly) {
			void	*vp{} ;
	                if ((rs = vecobj_get(op->mlp,mi,&vp)) >= 0) {
	                    MB_MI	*mp = (MB_MI *) vp ;
	                    f_prev = mp->cmd.msgdel ;
	                    if (! LEQUIV(f,f_prev)) {
	                        if (mp->cmd.msgdel) {
	                            op->msgs_del -= 1 ;
	                            mp->cmd.msgdel = false ;
	                        } else {
	                            op->msgs_del += 1 ;
	                            mp->cmd.msgdel = true ;
	                        }
	                    }
	                } /* end if (vecobj_get) */
	            } /* end if (writable-filesystem) */
	        } /* end if (found) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f_prev : rs ;
}
/* end subroutine (mailbox_msgdel) */

int mailbox_countdel(mailbox *op) noex {
	int		rs ;
	int		count = 0 ;
	if ((rs = mailbox_magic(op)) >= 0) {
	    count = op->msgs_del ;
	} /* end if (magic) */
	return (rs >= 0) ? count : rs ;
}
/* end subroutine (mailbox_countdel) */

int mailbox_msgoff(mailbox *op,int mi,off_t *offp) noex {
	int		rs ;
	int		mlen = 0 ;
	if ((rs = mailbox_magic(op,offp)) >= 0) {
	    rs = SR_INVALID ;
	    if (mi >= 0) {
		rs = SR_NOTFOUND ;
	        if (mi < op->msgs_total) {
	            void	*vp{} ;
	            if ((rs = vecobj_get(op->mlp,mi,&vp)) >= 0) {
	                MB_MI	*mp = (MB_MI *) vp ;
	                *offp = mp->hoff ;
	                mlen = mp->mlen ;
	            } else {
	                *offp = 0 ;
	            }
	        } /* end if (found) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? mlen : rs ;
}
/* end subroutine (mailbox_msgoff) */

int mailbox_msgret(mailbox *op,int mi,MB_MI **rpp) noex {
	int		rs ;
	int		mlen = 0 ;
	if ((rs = mailbox_magic(op,rpp)) >= 0) {
	    rs = SR_INVALID ;
	    if (mi >= 0) {
	        rs = SR_NOTFOUND ;
	        if (mi < op->msgs_total) {
	            void	*vp{} ;
	            if ((rs = vecobj_get(op->mlp,mi,&vp)) >= 0) {
	                MB_MI	*mp = (MB_MI *) vp ;
	                *rpp = mp ;
	                mlen = mp->mlen ;
	            }
	        } /* end if (found) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? mlen : rs ;
}
/* end subroutine (mailbox_msgret) */

int mailbox_msghdradd(mailbox *op,int mi,cchar *k,cchar *sp,int sl) noex {
	int		rs ;
	int		rv = 0 ;
	if ((rs = mailbox_magic(op,k)) >= 0) {
	    rs = SR_INVALID ;
	    if (k[0]) {
	        void	*vp{} ;
	        if ((rs = vecobj_get(op->mlp,mi,&vp)) >= 0) {
	            MB_MI	*mp = (MB_MI *) vp ;
	            if (! mp->f.hdradds) {
	                mp->f.hdradds = true ;
	                rs = vecstr_start(&mp->hdradds,1,0) ;
	            }
	            if (rs >= 0) {
	                op->f.rewrite = true ;
	                mp->f.addany = true ;
	                rs = vecstr_envadd(&mp->hdradds,k,sp,sl) ;
		        rv = rs ;
	            }
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (mailbox_msghdradd) */

int mailbox_readbegin(mailbox *op,MAILBOX_READ *curp,off_t foff,int rsz) noex {
	int		rs ;
	if ((rs = mailbox_magic(op,curp)) >= 0) {
	    memclear(curp) ;
	    if (rsz <= 0) rsz = MAILBOX_READSIZE ;
	    char	*p{} ;
	    if ((rs = uc_malloc(rsz,&p)) >= 0) {
	        curp->rbuf = p ;
	        curp->rsize = rsz ;
	        curp->foff = foff ;
	        curp->roff = foff ;
	    } /* end if (m-a) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailbox_readbegin) */

int mailbox_readend(mailbox *op,MAILBOX_READ *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mailbox_magic(op,curp)) >= 0) {
	    rs = SR_BADFD ;
	    if (curp->rbuf) {
	        rs1 = uc_free(curp->rbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        curp->rbuf = nullptr ;
	        curp->rbp = nullptr ;
	        curp->rsize = 0 ;
	        curp->rlen = 0 ;
	        curp->foff = 0 ;
	        curp->roff = 0 ;
	    } /* end if (good-fd) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailbox_readend) */

int mailbox_readln(mailbox *op,MAILBOX_READ *curp,char *lbuf,int llen) noex {
	int		rs ;
	int		tlen = 0 ;
	if ((rs = mailbox_magic(op,curp,lbuf)) >= 0) {
	    cint	mfd = op->mfd ;
	    int		i{} ;
	    int		mlen ;
	    char		*lbp = lbuf ;
	    char		*bp, *lastp ;
	    lbuf[0] = '\0' ;
	    while (tlen < llen) {
	        if (curp->rlen == 0) {
	            const off_t	po = (curp->roff+tlen) ;
		    cint	rbs = curp->rsize ;
		    char	*rbp = curp->rbuf ;
	            curp->rbp = curp->rbuf ;
	            if ((rs = u_pread(mfd,rbp,rbs,po)) >= 0) {
	                curp->rlen = rs ;
	                curp->foff += rs ;
	            }
	        } /* end if (refilling up buffer) */
	        if (rs < 0) break ;
	        if (curp->rlen == 0) break ;
	        mlen = min(curp->rlen,(llen - tlen)) ;
	        {
	            bp = curp->rbp ;
	            lastp = (curp->rbp + mlen) ;
	            while (bp < lastp) {
	                if ((*lbp++ = *bp++) == '\n') break ;
	            } /* end while */
	            i = (bp - curp->rbp) ;
	        } /* end block */
	        curp->rbp += i ;
	        tlen += i ;
	        curp->rlen -= i ;
	        if ((i > 0) && (lbp[-1] == '\n')) break ;
	    } /* end while (trying to satisfy request) */
	    if (rs >= 0) {
	        curp->roff += tlen ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (mailbox_readln) */


/* private subroutines */

static int mailbox_opener(mailbox *op,cc *mbfname,int of) noex {
	int		rs ;
	int		nmsgs = 0 ;
	if ((rs = uc_open(mbfname,of,0666)) >= 0) {
	    USTAT	sb ;
	    op->mfd = rs ;
	    if ((rs = u_fstat(op->mfd,&sb)) >= 0) {
	        if (S_ISREG(sb.st_mode)) {
		    cchar	*cp{} ;
	            op->mblen = (uint) sb.st_size ;
	            op->ti_mod = sb.st_mtime ;
	            op->ti_check = time(nullptr) ;
	            if ((rs = uc_mallocstrw(mbfname,-1,&cp)) >= 0) {
	                cint	es = sizeof(mailbox_msginfo) ;
	                cint	vo = VECOBJ_OCOMPACT ;
	                op->mailfname = cp ;
	                nmsgs = MAILBOX_DEFMSGS ;
	                if ((rs = vecobj_start(op->mlp,es,10,vo)) >= 0) {
	                    cint	to = op->to_lock ;
	                    if ((rs = uc_locktail(op->mfd,true,1,to)) >= 0) {
	                        off_t	loff ;
	                        if ((rs = mailbox_parse(op)) >= 0) {
	                            nmsgs = op->msgs_total ;
	                            op->magic = MAILBOX_MAGIC ;
	                        }
	                        if (rs < 0) {
	                            mailbox_msgfins(op) ;
	                        }
	                        loff = op->mblen ;
	                        lockfile(op->mfd,F_UNLOCK,loff,0L,0) ;
	                    } /* end if (lock) */
	                    if (rs < 0) {
	                        vecobj_finish(op->mlp) ;
			    }
	                } /* end if (vecstr-msgs) */
	                if (rs < 0) {
	                    uc_free(op->mailfname) ;
	                    op->mailfname = nullptr ;
	                }
	            } /* end if (m-a) */
	        } else {
	            rs = SR_ISDIR ;
	        }
	    } /* end if (stat) */
	    if (rs < 0) {
	        u_close(op->mfd) ;
	        op->mfd = -1 ;
	    }
	} /* end if (file-open) */
	return (rs >= 0) ? nmsgs : rs ;
}
/* end subroutine (mailbox_opener) */

static int mailbox_parse(mailbox *op) noex {
	fbliner		ls, *lsp = &ls ;
	filer		fb ;
	const off_t	soff = 0L ;
	cint		bsize = (32 * op->pagesize) ;
	int		rs ;
	int		rs1 ;
	int		to = -1 ;
	if_constexpr (f_readto) {
	    to = op->to_read ;
	}
	if ((rs = filer_start(&fb,op->mfd,soff,bsize,0)) >= 0) {
	    if ((rs = fbliner_start(lsp,&fb,soff,to)) >= 0) {
	        int	mi = 0 ;
	        while ((rs = mailbox_parsemsg(op,lsp,mi)) > 0) {
	            mi += 1 ;
	        } /* end while */
	        op->mblen = (lsp->foff - soff) ;
	        op->msgs_total = mi ;
	        rs1 = fbliner_finish(lsp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (liner) */
	    rs1 = filer_finish(&fb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filer) */
	return (rs >= 0) ? op->msgs_total : rs ;
}
/* end subroutine (mailbox_parse) */

static int mailbox_parsemsg(mailbox *op,fbliner *lsp,int mi) noex {
	MAILBOXPI	pi ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		ll = 0 ;
	if ((rs = mailboxpi_start(&pi,lsp,mi)) >= 0) {
	    mmenvdat	me ;
	    int		vi = 0 ;
	    cchar	*lp ;
/* find message start */
	    while ((rs = fbliner_getln(lsp,&lp)) >= 0) {
	        ll = rs ;
	        if (ll == 0) break ;
	        if ((rs >= 0) && (ll > 5) && FMAT(lp) &&
	            ((rs = mailmsgmatenv(&me,lp,ll)) > 0)) {
	            pi.f.fenv = true ;
	        } else if ((rs >= 0) && (ll > 2) &&
	            ((rs = mailmsgmathdr(lp,ll,&vi)) > 0)) {
	            pi.f.fhdr = true ;
	        } else if ((rs >= 0) && (ll <= 2) && (mi == 0)) {
	            if ((lp[0] == '\n') || hasEOH(lp,ll)) {
	                pi.f.feoh = true ;
	            }
	        } /* end if */
	        if (rs < 0) break ;
	        if (pi.f.fenv || pi.f.fhdr) break ;
	        ll = 0 ;
	        fbliner_done(lsp) ;
	        if (pi.f.feoh) break ;
	    } /* end while */
	    if ((rs >= 0) && mailboxpi_havemsg(&pi)) {
	        rs = mailbox_parsemsger(op,&me,&pi) ;
	        ll = rs ;
	    }
	    rs1 = mailboxpi_finish(&pi) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (mailboxpi) */
	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (mailbox_parsemsg) */

static int mailbox_parsemsger(mailbox *op,mmenvdat *mep,
		MAILBOXPI *pip) noex {
	fbliner		*lsp = pip->lsp ;
	MB_MI		msg, *msgp = &msg ;
	mailmsghdrval	mhv ;
	cint		mi = pip->mi ;
	int		rs ;
	int		rs1 ;
	int		ll = 0 ;
	cchar		*lp ;
	if ((rs = msginfo_start(msgp,lsp->poff,mi)) >= 0) {
	    int		hi = 0 ;
	    int		kl = 0 ;
	    int		vl = 0 ;
	    int		clen = -1 ;
	    int		clines = -1 ;
	    int		vi ;
	    cchar	*vp ;

	    pip->f.fmsg = true ;
	    if (pip->f.fenv) {
	        if ((rs = msginfo_setenv(msgp,mep)) >= 0) {
	            fbliner_done(lsp) ;
	        }
	    }

/* read headers (ignoring envelope) */

	    while ((rs >= 0) && ((rs = fbliner_getln(lsp,&lp)) >= 0)) {
	        ll = rs ;
	        if (ll == 0) break ;
	        if ((ll > 2) && (! pip->f.fenv) && 
	            (! pip->f.fhdr) && (! pip->f.feoh)) {
	            kl = mailmsgmathdr(lp,ll,&vi) ;
	            pip->f.fhdr = (kl > 0) ;
	        }

	        if ((rs >= 0) && pip->f.fhdr) {
	            pip->f.fhdr = false ;
	            if (pip->f.fmhv) { /* previous value outstanding */
	                rs = mailbox_loadmsghead(op,msgp,&mhv) ;
	                pip->f.fmhv = false ;
	                mailmsghdrval_finish(&mhv) ;
	            } /* end if (had a previous value outstanding) */

	            if (msgp->hoff < 0) {
	                msgp->hoff = lsp->poff ;
		    }

	            if (rs >= 0) {
	                hi = matcasestr(msghdrkeys,lp,kl) ;
	            }

	            if ((rs >= 0) && (hi >= 0)) {
	                vp = (lp + vi) ;
	                vl = (ll - vi) ;
	                pip->f.fmhv = true ;
	                rs = mailmsghdrval_start(&mhv,hi,vp,vl) ;
	            } /* end if (have one we want) */

	        } else if ((rs >= 0) && (ll > 1) && MSGHEADCONT(lp[0])) {
	            if (pip->f.fmhv) {
	                rs = mailmsghdrval_add(&mhv,lp,ll) ;
	            }
	        } else if ((rs >= 0) && ((lp[0] == '\n') || hasEOH(lp,ll))) {
	            if (pip->f.fmhv) { /* previous value outstanding */
	                rs = mailbox_loadmsghead(op,msgp,&mhv) ;
	                pip->f.fmhv = false ;
	                mailmsghdrval_finish(&mhv) ;
	            } /* end if (had a previous value outstanding) */

	            pip->f.feoh = true ;
	            if (msgp->hoff < 0)
	                msgp->hoff = lsp->poff ;

	            msgp->hlen = (lsp->poff - msgp->hoff) ;

	        } /* end if */

	        ll = 0 ;
	        pip->f.fenv = false ;
	        pip->f.fhdr = false ;
	        fbliner_done(lsp) ;

	        if (pip->f.feoh) break ;
	        if (rs < 0) break ;
	    } /* end while (reading lines) */

/* load last header */

	    if (rs >= 0) {

	        if (pip->f.fmhv) {
	            if (rs >= 0) {
	                rs = mailbox_loadmsghead(op,msgp,&mhv) ;
	            }
	            pip->f.fmhv = false ;
	            mailmsghdrval_finish(&mhv) ;
	        } /* end if */

	        if (msgp->hoff < 0) {
	            msgp->hoff = lsp->poff ;
		}

	        if (msgp->hlen < 0) {
	            msgp->hlen = (lsp->poff - msgp->hoff) ;
		}

	        if (pip->f.feoh) {
	            msgp->boff = lsp->foff ;
	        }

	    } /* end if (loading last header) */

/* handle reading the body */

	    if (rs >= 0) {

	        if (op->f.useclen && msgp->hdrval.clen) {
	            clen = msgp->clen ;
	            rs = fbliner_seek(lsp,msgp->clen) ;
	        } else {
	            int		linemax = INT_MAX ;
	            if (op->f.useclines && 
	                msgp->hdrval.clines && (msgp->clines >= 0))
	                linemax = msgp->clines ;
	            pip->f.fbol = true ;
	            clines = 0 ;
	            while ((rs >= 0) && (clines < linemax) && 
	                ((rs = fbliner_getln(lsp,&lp)) >= 0)) {
	                ll = rs ;
	                if (ll == 0) break ;
	                pip->f.feol = (lp[ll-1] == '\n') ;
	                if (pip->f.fbol && FMAT(lp) && (ll > 5)) {
	                    if ((rs = mailmsgmatenv(mep,lp,ll)) > 0) {
	                        pip->f.fenv = true ;
	                    }
	                    if (pip->f.fenv) break ;
	                }
	                ll = 0 ;
	                if (pip->f.feol) {
	                    clines += 1 ;
			}
	                pip->f.fbol = pip->f.feol ;
	                fbliner_done(lsp) ;
	            } /* end while (searching for new start-msg) */

	            if ((rs >= 0) && msgp->hdrval.clines) {
	                if (clines < msgp->clines) {
	                    msgp->clines = clines ;
	                }
	            }

	            clen = (lsp->poff - msgp->boff) ;

	        } /* end if (advancing to end-of-message) */

	        if (rs >= 0) {
		    if (msgp->clen < 0) msgp->clen = clen ; /* ???? */

	            if (! msgp->f.blen) {
	                msgp->f.blen = true ;
	                msgp->blen = (lsp->poff - msgp->boff) ;
	            } /* end if (blen) */

	            if (! msgp->f.mlen) {
	                msgp->f.mlen = true ;
	                msgp->mlen = (lsp->poff - msgp->moff) ;
	            } /* end if (mlen) */

	        } /* end if (ok) */

	    } /* end if (ok) */

/* finish off the last message */

	    if (rs >= 0) {
	        if ((! msgp->hdrval.clines) && (clines >= 0)) {
	            msgp->hdrval.clines = true ;
	            msgp->clines = clines ;
	        } /* end if (clines) */

	    } /* end if */

/* finish up */

	    if ((rs >= 0) && pip->f.fmsg) {
	        ll = msgp->mlen ;
	        msgp->msgi = mi ;
	        rs = vecobj_add(op->mlp,msgp) ;
	        if (rs >= 0) pip->f.fmsg = false ;
	    } /* end if (insertion) */

	    rs1 = msginfo_finish(msgp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (msginfo) */

	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (mailbox_parsemsger) */

static int mailbox_loadmsghead(mailbox *op,MB_MI *msgp,
		mailmsghdrval *mhvp) noex {
	int		rs  = SR_FAULT ;
	int		hi = 0 ;
	if (op && msgp && mhvp) {
	    int		vl = -1 ;
	    cchar	*vp{} ;
	    if ((rs = mailmsghdrval_get(mhvp,&vp,&vl)) >= 0) {
	        int	v = -1 ;
	        hi = rs ;
	        switch (hi) {
	        case headkey_clen:
	            msgp->hdr.clen = true ;
	            if ((vl > 0) && ((v = hdrextnum(vp,vl)) >= 0)) {
	                msgp->hdrval.clen = true ;
	                msgp->clen = v ;
	            }
	            break ;
	        case headkey_clines:
	            msgp->hdr.clines = true ;
	            if ((vl > 0) && ((v = hdrextnum(vp,vl)) >= 0)) {
	                msgp->hdrval.clines = true ;
	                msgp->clines = v ;
	            }
	            break ;
	        case headkey_lines:
	            msgp->hdr.lines = true ;
	            if ((vl > 0) && ((v = hdrextnum(vp,vl)) >= 0)) {
	                msgp->hdrval.lines = true ;
	                msgp->lines = v ;
	            }
	            break ;
	        case headkey_xlines:
	            msgp->hdr.xlines = true ;
	            if ((vl > 0) && ((v = hdrextnum(vp,vl)) >= 0)) {
	                msgp->hdrval.xlines = true ;
	                msgp->xlines = v ;
	            }
	            break ;
	        } /* end switch */
	    } /* end if (ok) */
	} /* end if (non-null) */
	return (rs >= 0) ? hi : rs ;
}
/* end subroutine (mailbox_loadmsghead) */

static int mailbox_msgfins(mailbox *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ; /* used afterwards */
	void		*vp{} ;
	for (i = 0 ; vecobj_get(op->mlp,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
		MB_MI	*mp = (MB_MI *) vp ;
	        rs1 = msginfo_finish(mp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mailbox_msgfins) */

static int mailbox_rewrite(mailbox *op) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	char		*dbuf{} ;
	if ((rs = malloc_mp(&dbuf)) >= 0) {
	    cint	dlen = rs ;
	    if ((rs = tmpmailboxes(dbuf,dlen)) >= 0) {
	        cchar	*mb = "mbXXXXXXXXXXXX" ;
	        char	*tpat{} ;
		if ((rs = malloc_mp(&tpat)) >= 0) {
	            if ((rs = mkpath2(tpat,dbuf,mb)) >= 0) {
		        rs = mailbox_rewrites(op,tpat) ;
		        rv = rs ;
	            } /* end if (mkpath) */
		    rs1 = uc_free(tpat) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (tmpmailboxes) */
	    rs1 = uc_free(dbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (mailbox_rewrite) */

static int mailbox_rewrites(mailbox *op,cchar *tpat) noex {
	sigblocker	ss ;
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if ((rs = sigblocker_start(&ss,sigblockers)) >= 0) {
	    cint	of = (O_RDWR | O_CREAT) ;
	    cmode	om = 0600 ;
	    char	*tbuf{} ;
	    if ((rs = malloc_mp(&tbuf)) >= 0) {
	        if ((rs = opentmpfile(tpat,of,om,tbuf)) >= 0) {
	            cint	tfd = rs ;
		    {
	                rs = mailbox_rewriter(op,tfd) ;
			rv = rs ;
		    }
	            u_close(tfd) ;
	            u_unlink(tbuf) ;
	        } /* end if (open tmp-file) */
	        rs1 = uc_free(tbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	    rs1 = sigblocker_finish(&ss) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblocker) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (mailbox_rewrites) */

static int mailbox_rewriter(mailbox *op,int tfd) noex {
	cint		ps = op->pagesize ;
	int		rs ;
	int		rs1 ;
	int		bl = op->pagesize ;
	int		wlen = 0 ;
	char		*bp{} ;
	if ((rs = uc_valloc(bl,&bp)) >= 0) {
	    MSGCOPY	mc{} ;
	    filer	fb, *fbp = &fb ;
	    off_t	mbchange = -1 ;
	    cint	mfd = op->mfd ;
	    cint	bsize = min(iceil(op->mblen,ps),(16*ps)) ;
	    int		elen = 0 ; /* GCC false complaint */
	    mc.fbp = fbp ;
	    mc.bp = bp ;
	    mc.bl = bl ;
	    mc.moff = 0 ;
	    if ((rs = filer_start(fbp,tfd,0L,bsize,0)) >= 0) {
	        for (int mi = 0 ; mi < op->msgs_total ; mi += 1) {
		    bool	f = false ;
		    bool	fcopy = false ;
		    bool	fdel = false ;
		    void	*vp{} ;
	            if ((rs = vecobj_get(op->mlp,mi,&vp)) >= 0) {
		        MB_MI	*mip = (MB_MI *) vp ;
	                fdel = mip->cmd.msgdel ;
	                f = fdel || mip->f.addany ;
	                if (f) {
	                    if (mbchange < 0) {
	                        mbchange = mip->moff ;
	                        fcopy = true ;
	                    }
	                }
	                if (fcopy) {
	                    if (! fdel) {
	                        rs = mailbox_msgcopy(op,&mc,mip) ;
	                        wlen += rs ;
	                    }
	                } /* end if */
		    } /* end if (vecobj_get) */
	            if (rs < 0) break ;
	        } /* end for */
	        if (rs >= 0) {
	            if (mc.moff != op->mblen) {
	                u_seek(mfd,op->mblen,SEEK_SET) ;
	                mc.moff = op->mblen ;
	            }
	            rs = filer_writefd(fbp,bp,bl,mfd,-1) ;
	            elen = rs ;
	            wlen += rs ;
	        } /* end if (finishing off) */
	        rs1 = filer_finish(fbp) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (filer) */
	    if (rs >= 0) {
/* extend the mailbox file if necessary (rarely happens?) */
	        if ((op->mblen + elen) < (mbchange + wlen)) {
	            rs1 = (mbchange + wlen) - (op->mblen + elen) ;
	            rs = writeblanks(mfd,rs1) ;
	        } /* end if */
/* copy adjusted data back to the mailbox file */
	        if ((rs >= 0) && (wlen > 0)) {
	            u_seek(mfd,mbchange,SEEK_SET) ;
	            u_seek(tfd,0L,SEEK_SET) ;
	            rs = uc_writedesc(mfd,tfd,-1) ;
	        }
/* truncate mailbox file if necessary */
	        if ((rs >= 0) && (op->mblen > (mbchange + wlen))) {
	            rs = uc_ftruncate(mfd,(mbchange+wlen)) ;
	        }
	    } /* end if (ok) */
	    rs1 = uc_free(bp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (mailbox_rewriter) */

static int mailbox_msgcopy(mailbox *op,MSGCOPY *mcp,MB_MI *mip) noex {
	int		rs = SR_OK ;
	int		mfd = op->mfd ;
	int		wlen = 0 ;
	if (mcp->moff != mip->moff) {
	    rs = u_seek(mfd,mip->moff,SEEK_SET) ;
	    mcp->moff = mip->moff ;
	}
	if (rs >= 0) {
	    if (mip->f.addany) {
	        rs = mailbox_msgcopyadd(op,mcp,mip) ;
	        wlen += rs ;
	    } else {
	        rs = filer_writefd(mcp->fbp,mcp->bp,mcp->bl,mfd,mip->mlen) ;
	        wlen += rs ;
	    }
	    mcp->moff += wlen ;
	} /* end if (ok) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mailbox_msgcopy) */

static int mailbox_msgcopyadd(mailbox *op,MSGCOPY *mcp,MB_MI *mip) noex {
	cint		mfd = op->mfd ;
	cint		ehlen = ((mip->hoff + mip->hlen) - mip->moff) ;
	int		rs ;
	int		wlen = 0 ;
	int		wl = mcp->bl ;
	char		*wp = mcp->bp ;
	if ((rs = filer_writefd(mcp->fbp,wp,wl,mfd,ehlen)) >= 0) {
	    wlen += rs ;
	    if (mip->f.addany && mip->f.hdradds) {
		vecstr	*hlp = &mip->hdradds ;
	        cchar	*sp{} ;
	        for (int i = 0 ; vecstr_get(hlp,i,&sp) >= 0 ; i += 1) {
	            if (sp) {
	                rs = filer_writehdr(mcp->fbp,sp,-1) ;
	                wlen += rs ;
		    }
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if */
	    if (rs >= 0) {
		wl = mcp->bl ;
		wp = mcp->bp ;
	        rs = filer_writefd(mcp->fbp,wp,wl,mfd,(mip->blen+1)) ;
	        wlen += rs ;
	    }
	} /* end if (filer_writefd) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mailbox_msgcopyadd) */

static int msginfo_start(MB_MI *mp,off_t off,int mi) noex {
	int		rs = SR_OK ;	
	if (mp) {
	    rs = memclear(mp) ;
	    mp->msgi = mi ;
	    mp->moff = off ;
	    mp->hoff = -1 ;
	    mp->soff = -1 ;
	    mp->boff = -1 ;
	    mp->hlen = -1 ;
	    mp->blen = -1 ;
	    mp->blen = -1 ;
	    mp->clen = -1 ;
	    mp->clines = -1 ;
	    mp->lines = -1 ;
#ifdef	COMMENT
	    mp->add.sem = true ;
	    mp->add.clen = true ;
	    mp->add.clines = true ;
	    mp->add.status = true ;
#endif
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (msginfo_start) */

static int msginfo_finish(MB_MI *mp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (mp->f.hdradds) {
	    mp->f.hdradds = false ;
	    rs1 = vecstr_finish(&mp->hdradds) ;
	    if (rs >= 0) rs = rs1 ;
	}
	mp->mlen = 0 ;
	return rs ;
}
/* end subroutine (msginfo_finish) */

static int msginfo_setenv(MB_MI *msgp,mmenvdat *mep) noex {
	int		rs = SR_OK ;
	msgp->f.env = true ;
	msgp->f.senv = mep->f.start ;
	return rs ;
}
/* end subroutine (msginfo_setenv) */

static int mailboxpi_finish(MAILBOXPI *pip) noex {
	int		rs = SR_FAULT ;
	if (pip) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailboxpi_finish) */

static int mailboxpi_havemsg(MAILBOXPI *pip) noex {
	return (pip->f.fenv || pip->f.fhdr || pip->f.feoh) ;
}
/* end subroutine (mailboxpi_havemsg) */

static int mailboxpi_start(MAILBOXPI *pip,fbliner *lsp,int mi) noex {
	int		rs = SR_FAULT ;
	if (pip && lsp) {
	    memclear(pip) ;		/* dangerous */
	    pip->lsp = lsp ;
	    pip->mi = mi ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailboxpi_start) */

static int writeblanks(int mfd,int len) noex {
	int		rs ;
	int		rs1 ;
	char		*lbuf{} ;
	if ((rs = malloc_ml(&lbuf)) >= 0) {
	    cint	llen = rs ;
	    int		rlen = len ;
	    int		ll ;
	    while ((rs >= 0) && (rlen > 0)) {
	        cint	maxlen = (rlen > 1) ? (rlen-1) : 0 ;
	        ll = min(llen,maxlen) ;
	        strnsetblanks(lbuf,ll) ;
	        lbuf[ll++] = '\n' ;
	        rs = uc_write(mfd,lbuf,ll) ;
	        rlen -= rs ;
	    } /* end while */
	    rs1 = uc_free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (writeblanks) */



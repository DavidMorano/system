/* mailbox_main SUPPORT */
/* charset=ISO8859-1 */
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

  	Object:
	nailbox

	Description:
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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<sysval.hh>
#include	<mallocxx.h>
#include	<sigblocker.h>
#include	<filer.h>
#include	<filer_mailsup.h>
#include	<fbliner.h>
#include	<ascii.h>
#include	<vecobj.h>
#include	<vecstr.h>
#include	<strn.h>		/* |strnset(3uc)| */
#include	<mkpathx.h>
#include	<sncpyx.h>
#include	<matxstr.h>
#include	<lockfile.h>
#include	<intceil.h>
#include	<hdrextnum.h>
#include	<opentmp.h>
#include	<strwcpy.h>		/* |strwblanks(3uc)| */
#include	<char.h>
#include	<hasx.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"mailbox.h"
#include	"mailmsghdrval.h"
#include	"mailmsghdrfold.h"
#include	"mailmsgmatenv.h"
#include	"mailmsgmathdr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */

#ifndef	MSGCOLS
#define	MSGCOLS			76
#endif

#define	BUFPAGEMULT		32	/* read buffer multiplier */

#define	MAILBOX_DEFMSGS		100
#define	MAILBOX_READSIZE	(4*1024)
#define	MAILBOX_TMPVARDNAME	"/var/tmp"
#define	MAILBOX_SUBDNAME	"mailboxes"

#define	MB_MI			mailbox_mi

#define	TO_LOCK		120		/* mailbox lock-timeout */
#define	TO_READ		5

#define	MSGHDRCONT(ch)	(((ch) == CH_SP) || ((ch) == CH_TAB))

#define	TMPDIRMODE	(0777 | S_ISVTX)

#ifndef	SIGACTION
#define	SIGACTION	struct sigaction
#endif

#define	MSGCOPY		msgcopy
#define	SIGSTATE	sigstate
#define	MAILBOXPI	mailboxpi
#define	MAILBOXPI_FL	mailboxpi_flags
#define	MMHV		mailmsghdrval

#ifndef	CF_READTO
#define	CF_READTO	0		/* use a read timeout? */
#endif


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libuc::libmem ;			/* variable */
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
	MAILBOXPI_FL	fl ;
	int		mi ;
} ;


/* forward references */

template<typename ... Args>
static int mailbox_ctor(mailbox *op,Args ... args) noex {
    	MAILBOX		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    memclear(hop) ;
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

static inline bool hasfmat(cc *lp,int ll) noex {
    	return ((ll > 5) && (lp[0] == 'F')) ;
}


/* local variables */

static sysval		pagesize(sysval_ps) ;

enum headkeys {
	headkey_clen,
	headkey_clines,
	headkey_lines,
	headkey_xlines,
	headkey_overlast
} ;

constexpr cpcchar	msghdrkeys[] = {
	"content-length",
	"content-lines",
	"lines",
	"x-lines",
	nullptr
} ;

constexpr int		sigblockers[] = {
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
		if ((rs = pagesize) >= 0) {
	            int		oflags = 0 ;
	            op->pagesize = rs ;
	            op->msgs_total = 0 ;
	            op->mflags = mflags ;
	            op->mfd = -1 ;
	            op->to_lock = TO_LOCK ;
	            op->to_read = TO_READ ;
	            op->fl.readonly = (! (mflags & MAILBOX_ORDWR)) ;
	            op->fl.useclen = (! (mflags & MAILBOX_ONOCLEN)) ;
	            op->fl.useclines = !!(mflags & MAILBOX_OUSECLINES) ;
	            oflags |= (mflags & MAILBOX_ORDWR) ? O_RDWR : 0 ;
		    rs = mailbox_opener(op,mbfname,oflags) ;
		    nmsgs = rs ;
		} /* end if (pageisze) */
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
	    f = f && (! op->fl.readonly) ;
	    f = f && ((op->msgs_del > 0) || op->fl.rewrite) ;
	    if (f) {
	        rs1 = mailbox_rewrite(op) ;
	        if (rs >= 0) rs = rs1 ;
	        mblen = rs ;
	    }
	    {
	        rs1 = mailbox_msgfins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->mlp) {
	        rs1 = vecobj_finish(op->mlp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->mailfname) {
		void *vp = voidp(op->mailfname) ;
	        rs1 = libmem.free(vp) ;
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
	    if (dt == 0) dt = getustime ;
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
	            if (! op->fl.readonly) {
			vecobj *mlp = op->mlp ;
			if (void *vp{} ; (rs = mlp->get(mi,&vp)) >= 0) {
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
    		    vecobj	*mlp = op->mlp ;
	            if (void *vp{} ; (rs = mlp->get(mi,&vp)) >= 0) {
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
    		    vecobj	*mlp = op->mlp ;
	            if (void *vp{} ; (rs = mlp->get(mi,&vp)) >= 0) {
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
    		vecobj	*mlp = op->mlp ;
	        if (void *vp{} ; (rs = mlp->get(mi,&vp)) >= 0) {
	            MB_MI	*mp = (MB_MI *) vp ;
	            if (! mp->fl.hdradds) {
	                mp->fl.hdradds = true ;
	                rs = vecstr_start(&mp->hdradds,1,0) ;
	            }
	            if (rs >= 0) {
	                op->fl.rewrite = true ;
	                mp->fl.addany = true ;
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
	    if (char *p ; (rs = libmem.mall(rsz,&p)) >= 0) {
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
	        rs1 = libmem.free(curp->rbuf) ;
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
	    char	*lbp = lbuf ;
	    char	*bp ;
	    char	*lastp ;
	    lbuf[0] = '\0' ;
	    while (tlen < llen) {
	        if (curp->rlen == 0) {
	            coff	po = (curp->roff+tlen) ;
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
	            i = intconv(bp - curp->rbp) ;
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
	cmode		om = 0666 ;
	if ((rs = uc_open(mbfname,of,om)) >= 0) {
	    op->mfd = rs ;
	    if (ustat sb ; (rs = u_fstat(op->mfd,&sb)) >= 0) {
	        if (S_ISREG(sb.st_mode)) {
		    cchar	*fn = mbfname ;
	            op->mblen = intsat(sb.st_size) ;
	            op->ti_mod = sb.st_mtime ;
	            op->ti_check = getustime ;
		    if (cchar *cp ; (rs = libmem.strw(fn,-1,&cp)) >= 0) {
	                cint	es = szof(mailbox_msginfo) ;
			cint	vn = 10 ;
	                cint	vo = VECOBJ_OCOMPACT ;
	                op->mailfname = cp ;
	                nmsgs = MAILBOX_DEFMSGS ;
	                if ((rs = vecobj_start(op->mlp,es,vn,vo)) >= 0) {
	                    cint	to = op->to_lock ;
	                    if ((rs = uc_locktail(op->mfd,true,1,to)) >= 0) {
	                        if ((rs = mailbox_parse(op)) >= 0) {
	                            nmsgs = op->msgs_total ;
	                            op->magic = MAILBOX_MAGIC ;
	                        }
	                        if (rs < 0) {
	                            mailbox_msgfins(op) ;
	                        }
				{
	                            coff	loff = op->mblen ;
	                            lockfile(op->mfd,F_UNLOCK,loff,0L,0) ;
				}
	                    } /* end if (lock) */
	                    if (rs < 0) {
	                        vecobj_finish(op->mlp) ;
			    }
	                } /* end if (vecstr-msgs) */
	                if (rs < 0) {
			    void *vp = voidp(op->mailfname) ;
	                    libmem.free(vp) ;
	                    op->mailfname = nullptr ;
	                } /* end if (error) */
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
	coff		soff = 0z ;
	cint		bsz = (BUFPAGEMULT * op->pagesize) ;
	int		rs ;
	int		rs1 ;
	int		to = -1 ;
	if_constexpr (f_readto) {
	    to = op->to_read ;
	}
	if (filer fb ; (rs = fb.start(op->mfd,soff,bsz,0)) >= 0) {
	    fbliner	ls, *lsp = &ls ;
	    if ((rs = lsp->start(&fb,soff,to)) >= 0) {
	        int	mi = 0 ;
	        while ((rs = mailbox_parsemsg(op,lsp,mi)) > 0) {
	            mi += 1 ;
	        } /* end while */
	        op->mblen = intconv(lsp->foff - soff) ;
	        op->msgs_total = mi ;
	        rs1 = lsp->finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (liner) */
	    rs1 = fb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filer) */
	return (rs >= 0) ? op->msgs_total : rs ;
}
/* end subroutine (mailbox_parse) */

static int mailbox_parsemsg(mailbox *op,fbliner *lsp,int mi) noex {
	int		rs ;
	int		rs1 ;
	int		ll = 0 ;
	if (MAILBOXPI pi ; (rs = mailboxpi_start(&pi,lsp,mi)) >= 0) {
	    mmenvdat	me ;
	    int		vi = 0 ;
	    /* find message start */
	    for (cchar *lp ; (rs = lsp->getln(&lp)) > 0 ; ) {
	        ll = rs ;
	        if (hasfmat(lp,ll) && ((rs = mailmsgmatenv(&me,lp,ll)) > 0)) {
	            pi.fl.fenv = true ;
	        } else if ((ll > 2) && ((rs = mailmsgmathdr(lp,ll,&vi)) > 0)) {
	            pi.fl.fhdr = true ;
	        } else if ((ll <= 2) && (mi == 0)) {
	            if ((lp[0] == '\n') || hasEOH(lp,ll)) {
	                pi.fl.feoh = true ;
	            }
	        } /* end if */
	        if (rs < 0) break ;
	        if (pi.fl.fenv || pi.fl.fhdr) break ;
	        ll = 0 ;
	        rs1 = lsp->done() ;
		if (rs >= 0) rs = rs1 ;
	        if (pi.fl.feoh) break ;
		if (rs < 0) break ;
	    } /* end for */
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

static int mailbox_parsemsger(mailbox *op,mmenvdat *mep,MAILBOXPI *pip) noex {
	fbliner		*lsp = pip->lsp ;
	cint		mi = pip->mi ;
	int		rs ;
	int		rs1 ;
	int		ll = 0 ;
	MB_MI		msg, *msgp = &msg ;
	if ((rs = msginfo_start(msgp,lsp->poff,mi)) >= 0) {
	    mailmsghdrval	mhv ;
	    int		hi = 0 ;
	    int		kl = 0 ;
	    int		vl = 0 ;
	    int		clen = -1 ;
	    int		clines = -1 ;
	    int		vi ;
	    cchar	*lp ;
	    cchar	*vp ;
	    pip->fl.fmsg = true ;
	    if (pip->fl.fenv) {
	        if ((rs = msginfo_setenv(msgp,mep)) >= 0) {
	            rs = lsp->done ;
	        }
	    }
	    /* read headers (ignoring envelope) */
	    while ((rs >= 0) && ((rs = lsp->getln(&lp)) > 0)) {
	        ll = rs ;
	        if ((ll > 2) && (! pip->fl.fenv) && 
	            (! pip->fl.fhdr) && (! pip->fl.feoh)) {
	            kl = mailmsgmathdr(lp,ll,&vi) ;
	            pip->fl.fhdr = (kl > 0) ;
	        }
	        if ((rs >= 0) && pip->fl.fhdr) {
	            pip->fl.fhdr = false ;
	            if (pip->fl.fmhv) { /* previous value outstanding */
	                rs = mailbox_loadmsghead(op,msgp,&mhv) ;
	                mailmsghdrval_finish(&mhv) ;
	                pip->fl.fmhv = false ;
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
	                if ((rs = mailmsghdrval_start(&mhv,hi,vp,vl)) >= 0) {
	                    pip->fl.fmhv = true ;
			}
	            } /* end if (have one we want) */
	        } else if ((rs >= 0) && (ll > 1) && MSGHDRCONT(lp[0])) {
	            if (pip->fl.fmhv) {
	                rs = mailmsghdrval_add(&mhv,lp,ll) ;
	            }
	        } else if ((rs >= 0) && ((lp[0] == '\n') || hasEOH(lp,ll))) {
	            if (pip->fl.fmhv) { /* previous value outstanding */
	                rs = mailbox_loadmsghead(op,msgp,&mhv) ;
	                mailmsghdrval_finish(&mhv) ;
	                pip->fl.fmhv = false ;
	            } /* end if (had a previous value outstanding) */
	            pip->fl.feoh = true ;
	            if (msgp->hoff < 0) {
	                msgp->hoff = lsp->poff ;
		    }
	            msgp->hlen = intconv(lsp->poff - msgp->hoff) ;
	        } /* end if */
	        ll = 0 ;
	        pip->fl.fenv = false ;
	        pip->fl.fhdr = false ;
	        rs1 = lsp->done ;
		if (rs >= 0) rs = rs1 ;
	        if (pip->fl.feoh) break ;
	        if (rs < 0) break ;
	    } /* end while (reading lines) */
	    /* load last header */
	    if (rs >= 0) {
	        if (pip->fl.fmhv) {
	            if (rs >= 0) {
	                rs = mailbox_loadmsghead(op,msgp,&mhv) ;
	            }
	            pip->fl.fmhv = false ;
	            mailmsghdrval_finish(&mhv) ;
	        } /* end if */
	        if (msgp->hoff < 0) {
	            msgp->hoff = lsp->poff ;
		}
	        if (msgp->hlen < 0) {
	            msgp->hlen = intconv(lsp->poff - msgp->hoff) ;
		}
	        if (pip->fl.feoh) {
	            msgp->boff = lsp->foff ;
	        }
	    } /* end if (loading last header) */
	    /* handle reading the body */
	    if (rs >= 0) {
	        if (op->fl.useclen && msgp->hdrval.clen) {
	            clen = msgp->clen ;
	            rs = lsp->adv(msgp->clen) ;
	        } else {
	            int		linemax = INT_MAX ;
		    bool	f = op->fl.useclines ;
		    f = f || msgp->hdrval.clines ;
		    f = f || (msgp->clines >= 0) ;
		    if (f) {
	                linemax = msgp->clines ;
		    }
	            pip->fl.fbol = true ;
	            clines = 0 ;
		    auto getln = [&clines,&linemax,&lsp] (cchar **lpp) noex { 
			int	rsl = SR_OK ; /* XXX GCC "shadow" complaint */
			if (clines < linemax) {
	                    rsl = lsp->getln(lpp) ;
			}
			return rsl ;
		    } ; /* end lambda (getln) */
		    while ((rs >= 0) && ((rs = getln(&lp)) > 0)) {
	                ll = rs ;
	                pip->fl.feol = (lp[ll-1] == '\n') ;
	                if (pip->fl.fbol && hasfmat(lp,ll)) {
	                    if ((rs = mailmsgmatenv(mep,lp,ll)) > 0) {
	                        pip->fl.fenv = true ;
	                    }
	                    if (pip->fl.fenv) break ;
	                }
	                ll = 0 ;
	                if (pip->fl.feol) {
	                    clines += 1 ;
			}
	                pip->fl.fbol = pip->fl.feol ;
	                rs1 = lsp->done ;
			if (rs >= 0) rs = rs1 ;
	            } /* end while (searching for new start-msg) */
	            if ((rs >= 0) && msgp->hdrval.clines) {
	                if (clines < msgp->clines) {
	                    msgp->clines = clines ;
	                }
	            }
	            clen = intconv(lsp->poff - msgp->boff) ;
	        } /* end if (advancing to end-of-message) */
	        if (rs >= 0) {
		    if (msgp->clen < 0) {
			msgp->clen = clen ; /* ???? */
		    }
	            if (! msgp->fl.blen) {
	                msgp->fl.blen = true ;
	                msgp->blen = intconv(lsp->poff - msgp->boff) ;
	            } /* end if (blen) */
	            if (! msgp->fl.mlen) {
	                msgp->fl.mlen = true ;
	                msgp->mlen = intconv(lsp->poff - msgp->moff) ;
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
	    if ((rs >= 0) && pip->fl.fmsg) {
	        ll = msgp->mlen ;
	        msgp->msgi = mi ;
	        rs = vecobj_add(op->mlp,msgp) ;
	        if (rs >= 0) pip->fl.fmsg = false ;
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
	    if (cchar *vp{} ; (rs = mailmsghdrval_get(mhvp,&vp,&vl)) >= 0) {
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
    	vecobj		*mlp = op->mlp ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ; /* used-afterwards */
	void		*vp{} ;
	for (i = 0 ; mlp->get(i,&vp) >= 0 ; i += 1) {
	    MB_MI	*mp = (MB_MI *) vp ;
	    if (vp) {
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
	if (char *dbuf ; (rs = malloc_mp(&dbuf)) >= 0) {
	    cint	dlen = rs ;
	    if ((rs = tmpmailboxes(dbuf,dlen)) >= 0) {
	        cchar	*mb = "mbXXXXXXXXXXXX" ;
	        if (char *tpat ; (rs = malloc_mp(&tpat)) >= 0) {
	            if ((rs = mkpath2(tpat,dbuf,mb)) >= 0) {
		        rs = mailbox_rewrites(op,tpat) ;
		        rv = rs ;
	            } /* end if (mkpath) */
		    rs1 = malloc_free(tpat) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (tmpmailboxes) */
	    rs1 = malloc_free(dbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (mailbox_rewrite) */

static int mailbox_rewrites(mailbox *op,cchar *tpat) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if (sigblocker sb ; (rs = sb.start(sigblockers)) >= 0) {
	    cint	of = (O_RDWR | O_CREAT) ;
	    cmode	om = 0600 ;
	    if (char *tbuf ; (rs = malloc_mp(&tbuf)) >= 0) {
	        if ((rs = opentmpfile(tpat,of,om,tbuf)) >= 0) {
	            cint	tfd = rs ;
		    {
	                rs = mailbox_rewriter(op,tfd) ;
			rv = rs ;
		    }
	            u_close(tfd) ;
	            u_unlink(tbuf) ;
	        } /* end if (open tmp-file) */
	        rs1 = malloc_free(tbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	    rs1 = sb.finish ;
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
	if (char *bp ; (rs = libmem.vall(bl,&bp)) >= 0) {
	    filer	fb, *fbp = &fb ;
	    MSGCOPY	mc{} ;
	    off_t	mbchange = -1 ;
	    cint	mfd = op->mfd ;
	    cint	bsz = min(iceil(op->mblen,ps),(16*ps)) ;
	    int		elen = 0 ; /* GCC false complaint */
	    mc.bp = bp ;
	    mc.bl = bl ;
	    mc.moff = 0 ;
	    if ((rs = fbp->start(tfd,0z,bsz,0)) >= 0) {
    		vecobj		*mlp = op->mlp ;
	        mc.fbp = fbp ;
	        for (int mi = 0 ; mi < op->msgs_total ; mi += 1) {
		    bool	f = false ;
		    bool	fcopy = false ;
		    bool	fdel = false ;
		    if (void *vp{} ; (rs = mlp->get(mi,&vp)) >= 0) {
		        MB_MI	*mip = (MB_MI *) vp ;
	                fdel = mip->cmd.msgdel ;
	                f = fdel || mip->fl.addany ;
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
	            rs = fbp->writefd(bp,bl,mfd,-1) ;
	            elen = rs ;
	            wlen += rs ;
	        } /* end if (finishing off) */
	        rs1 = fbp->finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (filer) */
	    if (rs >= 0) {
		/* extend the mailbox file if necessary (rarely happens?) */
	        if ((op->mblen + elen) < (mbchange + wlen)) {
	            rs1 = intconv(mbchange + wlen) - (op->mblen + elen) ;
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
	    rs1 = libmem.free(bp) ;
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
	    if (mip->fl.addany) {
	        rs = mailbox_msgcopyadd(op,mcp,mip) ;
	        wlen += rs ;
	    } else {
		filer	*fbp = mcp->fbp ;
	        rs = fbp->writefd(mcp->bp,mcp->bl,mfd,mip->mlen) ;
	        wlen += rs ;
	    }
	    mcp->moff += wlen ;
	} /* end if (ok) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mailbox_msgcopy) */

static int mailbox_msgcopyadd(mailbox *op,MSGCOPY *mcp,MB_MI *mip) noex {
	cint		mfd = op->mfd ;
	cint		ehlen = intconv((mip->hoff + mip->hlen) - mip->moff) ;
	int		rs ;
	int		wlen = 0 ;
	int		wl = mcp->bl ;
	char		*wp = mcp->bp ;
	if (filer *fbp = mcp->fbp ; (rs = fbp->writefd(wp,wl,mfd,ehlen)) >= 0) {
	    wlen += rs ;
	    if (mip->fl.addany && mip->fl.hdradds) {
		vecstr	*hlp = &mip->hdradds ;
	        cchar	*sp{} ;
	        for (int i = 0 ; hlp->get(i,&sp) >= 0 ; i += 1) {
	            if (sp) {
	                rs = filer_writehdr(fbp,sp,-1) ;
	                wlen += rs ;
		    }
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if */
	    if (rs >= 0) {
		wl = mcp->bl ;
		wp = mcp->bp ;
	        rs = fbp->writefd(wp,wl,mfd,(mip->blen+1)) ;
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
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mp) {
	    rs = SR_OK ;
	    if (mp->fl.hdradds) {
	        mp->fl.hdradds = false ;
	        rs1 = vecstr_finish(&mp->hdradds) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    mp->mlen = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (msginfo_finish) */

static int msginfo_setenv(MB_MI *msgp,mmenvdat *mep) noex {
	int		rs = SR_FAULT ;
	if (msgp && mep) {
	    rs = SR_OK ;
	    msgp->fl.env = true ;
	    msgp->fl.senv = mep->fl.start ;
	}
	return rs ;
}
/* end subroutine (msginfo_setenv) */

static int mailboxpi_start(MAILBOXPI *pip,fbliner *lsp,int mi) noex {
	int		rs = SR_FAULT ;
	if (pip && lsp) {
	    rs = memclear(pip) ; /* dangerous */
	    pip->lsp = lsp ;
	    pip->mi = mi ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailboxpi_start) */

static int mailboxpi_finish(MAILBOXPI *pip) noex {
	int		rs = SR_FAULT ;
	if (pip) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailboxpi_finish) */

static int mailboxpi_havemsg(MAILBOXPI *pip) noex {
	return (pip->fl.fenv || pip->fl.fhdr || pip->fl.feoh) ;
}
/* end subroutine (mailboxpi_havemsg) */

static int writeblanks(int mfd,int len) noex {
	int		rs ;
	int		rs1 ;
	if (char *lbuf ; (rs = malloc_ml(&lbuf)) >= 0) {
	    cint	llen = rs ;
	    int		rlen = len ;
	    int		ll ;
	    while ((rs >= 0) && (rlen > 0)) {
	        cint	maxlen = (rlen > 1) ? (rlen-1) : 0 ;
	        ll = min(llen,maxlen) ;
	        strnblanks(lbuf,ll) ;
	        lbuf[ll++] = '\n' ;
	        rs = uc_write(mfd,lbuf,ll) ;
	        rlen -= rs ;
	    } /* end while */
	    rs1 = malloc_free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (writeblanks) */



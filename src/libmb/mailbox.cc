/* mailbox SUPPORT */
/* lang=C_++20 */

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
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<sigblocker.h>
#include	<mallocxx.h>
#include	<filebuf.h>
#include	<ascii.h>
#include	<vecobj.h>
#include	<vecstr.h>
#include	<strn.h>
#include	<mkpathx.h>
#include	<sncpyx.h>
#include	<matxstr.h>
#include	<lockfile.h>
#include	<intceil.h>
#include	<char.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"mailbox.h"
#include	"mailmsghdrval.h"
#include	"mailmsghdrfold.h"
#include	"mailmsgmatenv.h"
#include	"mailmsgmathdr.h"


/* local defines */

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN		MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN		2048
#endif
#endif

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

#define	LINER		struct liner

#define	MAILBOXPI	struct mailboxpi
#define	MAILBOXPI_FL	struct mailboxpi_flags

#define	MMHV		mailmsghdrval


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	filebuf_writefd(filebuf *,char *,int,int,int) noex ;
    extern int	filebuf_writehdr(filebuf *,cchar *,int) noex ;
    extern int	filebuf_writehdrval(filebuf *,cchar *,int) noex ;
}

extern "C" {
    extern int	tmpmailboxes(char *,int) noex ;
    extern int	opentmpfile(cchar *,int,mode_t,char *) noex ;
    extern int	hdrextnum(cchar *,int) noex ;
    extern int	hasEOH(cchar *,int) noex ;
}


/* external variables */


/* local structures */

struct liner {
	filebuf		*fbp ;		/* externally-owned */
	char		*lbuf ;		/* allocated */
	off_t		poff ;		/* file-offset previous */
	off_t		foff ;		/* file-offset current */
	int		to ;		/* read time-out */
	int		llen ;
} ;

struct sigstate {
	SIGACTION	*sao ;
	sigset_t	oldsigmask ;
} ;

struct msgcopy {
	filebuf		*fbp ;
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
	LINER		*lsp ;
	MAILBOXPI_FL	f ;
	int		mi ;
} ;

/* local (forward) subroutines */

static int mailbox_parse(mailbox *) noex ;
static int mailbox_parsemsg(mailbox *,LINER *,int) noex ;
static int mailbox_parsemsger(mailbox *,mailmsgenv *,MAILBOXPI *) noex ;
static int mailbox_loadmsghead(mailbox *,MB_MI *,
		mailmsghdrval *) noex ;
static int mailbox_msgfins(mailbox *) noex ;
static int mailbox_rewrite(mailbox *) noex ;
static int mailbox_rewriter(mailbox *,int) noex ;
static int mailbox_msgcopy(mailbox *,MSGCOPY *,MB_MI *) noex ;
static int mailbox_msgcopyadd(mailbox *,MSGCOPY *,MB_MI *) noex ;

static int msginfo_start(MB_MI *,off_t,int) noex ;
static int msginfo_finish(MB_MI *) noex ;
static int msginfo_setenv(MB_MI *,mailmsgenv *) noex ;

static int liner_start(LINER *,filebuf *,off_t,int) noex ;
static int liner_finish(LINER *) noex ;
static int liner_read(LINER *,cchar **) noex ;
static int liner_done(LINER *) noex ;
static int liner_seek(LINER *,int) noex ;

static int mailboxpi_start(MAILBOXPI *,LINER *,int) noex ;
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


/* exported variables */


/* exported subroutines */

int mailbox_open(mailbox *mbp,cchar *mbfname,int mflags) noex {
	USTAT		sb ;
	int		rs ;
	int		oflags ;
	int		nmsgs = 0 ;
	cchar	*cp ;

	if (mbp == nullptr) return SR_FAULT ;
	if (mbfname == nullptr) return SR_FAULT ;

	if (mbfname[0] == '\0') return SR_INVALID ;

	memclear(mbp) ;		/* dangerous */
	mbp->mfd = -1 ;
	mbp->msgs_total = 0 ;
	mbp->to_lock = TO_LOCK ;
	mbp->to_read = TO_READ ;
	mbp->mflags = mflags ;
	mbp->pagesize = getpagesize() ;

	mbp->f.readonly = (! (mflags & MAILBOX_ORDWR)) ;
	mbp->f.useclen = (! (mflags & MAILBOX_ONOCLEN)) ;
	mbp->f.useclines = MKBOOL(mflags & MAILBOX_OUSECLINES) ;

	oflags = 0 ;
	oflags |= (mflags & MAILBOX_ORDWR) ? O_RDWR : 0 ;

	if ((rs = uc_open(mbfname,oflags,0666)) >= 0) {
	    mbp->mfd = rs ;
	    if ((rs = u_fstat(mbp->mfd,&sb)) >= 0) {
	        if (S_ISREG(sb.st_mode)) {
	            mbp->mblen = (uint) sb.st_size ;
	            mbp->ti_mod = sb.st_mtime ;
	            mbp->ti_check = time(nullptr) ;
	            if ((rs = uc_mallocstrw(mbfname,-1,&cp)) >= 0) {
	                cint	es = sizeof(struct mailbox_msg) ;
	                cint	vo = VECOBJ_OCOMPACT ;
	                mbp->mailfname = cp ;
	                nmsgs = MAILBOX_DEFMSGS ;
	                if ((rs = vecobj_start(&mbp->msgs,es,10,vo)) >= 0) {
	                    cint	to = mbp->to_lock ;
	                    if ((rs = uc_locktail(mbp->mfd,true,1,to)) >= 0) {
	                        off_t	loff ;
	                        if ((rs = mailbox_parse(mbp)) >= 0) {
	                            nmsgs = mbp->msgs_total ;
	                            mbp->magic = MAILBOX_MAGIC ;
	                        }
	                        if (rs < 0) {
	                            mailbox_msgfins(mbp) ;
	                        }
	                        loff = mbp->mblen ;
	                        lockfile(mbp->mfd,F_UNLOCK,loff,0L,0) ;
	                    } /* end if (lock) */
	                    if (rs < 0)
	                        vecobj_finish(&mbp->msgs) ;
	                } /* end if (vecstr-msgs) */
	                if (rs < 0) {
	                    uc_free(mbp->mailfname) ;
	                    mbp->mailfname = nullptr ;
	                }
	            } /* end if (m-a) */
	        } else {
	            rs = SR_ISDIR ;
	        }
	    } /* end if (stat) */
	    if (rs < 0) {
	        u_close(mbp->mfd) ;
	        mbp->mfd = -1 ;
	    }
	} /* end if (file-open) */

	return (rs >= 0) ? nmsgs : rs ;
}
/* end subroutine (mailbox_open) */

int mailbox_close(mailbox *mbp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		mblen = 0 ;
	int		f ;

	if (mbp == nullptr) return SR_FAULT ;

	if (mbp->magic != MAILBOX_MAGIC) return SR_NOTOPEN ;

	f = (! mbp->f.readonly) ;
	f = f && ((mbp->msgs_del > 0) || mbp->f.rewrite) ;
	if (f) {
	    rs1 = mailbox_rewrite(mbp) ;
	    if (rs >= 0) rs = rs1 ;
	    mblen = rs ;
	}

/* free up everything */

	rs1 = mailbox_msgfins(mbp) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = vecobj_finish(&mbp->msgs) ;
	if (rs >= 0) rs = rs1 ;

	if (mbp->mailfname != nullptr) {
	    rs1 = uc_free(mbp->mailfname) ;
	    if (rs >= 0) rs = rs1 ;
	    mbp->mailfname = nullptr ;
	}

	if (mbp->mfd >= 0) {
	    rs1 = u_close(mbp->mfd) ;
	    if (rs >= 0) rs = rs1 ;
	    mbp->mfd = -1 ;
	}

	mbp->magic = 0 ;
	return (rs >= 0) ? mblen : rs ;
}
/* end subroutine (mailbox_close) */

int mailbox_mbfile(mailbox *mbp,char *mbuf,int mlen) noex {
	int		rs ;

	if (mbp == nullptr) return SR_FAULT ;
	if (mbuf == nullptr) return SR_FAULT ;

	if (mbp->magic != MAILBOX_MAGIC) return SR_NOTOPEN ;

	rs = sncpy1(mbuf,mlen,mbp->mailfname) ;

	return rs ;
}
/* end subroutine (mailbox_mbfile) */

int mailbox_info(mailbox *mbp,MAILBOX_INFO *mip) noex {
	if (mbp == nullptr) return SR_FAULT ;
	if (mip == nullptr) return SR_FAULT ;
	if (mbp->magic != MAILBOX_MAGIC) return SR_NOTOPEN ;

/* fill it in */

	memset(mip,0,sizeof(MAILBOX_INFO)) ;

	mip->nmsgs = mbp->msgs_total ;

	return mbp->msgs_total ;
}
/* end subroutine (mailbox_info) */

int mailbox_count(mailbox *mbp) noex {
	if (mbp == nullptr) return SR_FAULT ;

	if (mbp->magic != MAILBOX_MAGIC) return SR_NOTOPEN ;

	return mbp->msgs_total ;
}
/* end subroutine (mailbox_count) */

int mailbox_check(mailbox *mbp,time_t daytime) noex {
	int		rs = SR_OK ;
	int		f = false ;

	if (mbp == nullptr) return SR_FAULT ;

	if (mbp->magic != MAILBOX_MAGIC) return SR_NOTOPEN ;

	if (daytime == 0)
	    daytime = time(nullptr) ;

	if (daytime == 0)
	    f = true ;

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mailbox_check) */

int mailbox_msgdel(mailbox *mbp,int mi,int f) noex {
	MB_MI	*mp ;
	int		rs ;
	int		f_prev = false ;

	if (mbp == nullptr) return SR_FAULT ;

	if (mbp->magic != MAILBOX_MAGIC) return SR_NOTOPEN ;

	if (mi < 0) return SR_INVALID ;

	if (mi >= mbp->msgs_total)
	    return SR_NOTFOUND ;

	if (mbp->f.readonly)
	    return SR_ROFS ;

	if ((rs = vecobj_get(&mbp->msgs,mi,&mp)) >= 0) {
	    f_prev = mp->cmd.msgdel ;
	    if (! LEQUIV(f,f_prev)) {
	        if (mp->cmd.msgdel) {
	            mbp->msgs_del -= 1 ;
	            mp->cmd.msgdel = false ;
	        } else {
	            mbp->msgs_del += 1 ;
	            mp->cmd.msgdel = true ;
	        }
	    }
	} /* end if (vecobj_get) */

	return (rs >= 0) ? f_prev : rs ;
}
/* end subroutine (mailbox_msgdel) */

int mailbox_countdel(mailbox *mbp) noex {
	int		count = 0 ;

	if (mbp == nullptr) return SR_FAULT ;

	if (mbp->magic != MAILBOX_MAGIC) return SR_NOTOPEN ;

#ifdef	COMMENT
	{
	    MB_MI	*msgp ;
	    for (i = 0 ; vecobj_get(&mbp->msgs,i,&msgp) >= 0 ; i += 1) {
	        if (mp->f.msgdel) count += 1 ;
	    } /* end for */
	} /* end block */
#else
	count = mbp->msgs_del ;
#endif /* COMMENT */

	return count ;
}
/* end subroutine (mailbox_countdel) */

int mailbox_msgoff(mailbox *mbp,int mi,off_t *offp) noex {
	MB_MI	*mp ;
	int		rs ;
	int		mlen = 0 ;

	if (mbp == nullptr) return SR_FAULT ;
	if (offp == nullptr) return SR_FAULT ;

	if (mbp->magic != MAILBOX_MAGIC) return SR_NOTOPEN ;

	if (mi < 0) return SR_INVALID ;

	if (mi >= mbp->msgs_total) return SR_NOTFOUND ;

	if ((rs = vecobj_get(&mbp->msgs,mi,&mp)) >= 0) {
	    *offp = mp->hoff ;
	    mlen = mp->mlen ;
	} else
	    *offp = 0 ;

	return (rs >= 0) ? mlen : rs ;
}
/* end subroutine (mailbox_msgoff) */

int mailbox_msginfo(mailbox *mbp,int mi,MB_MI *mip) noex {
	MB_MI	*mp ;
	int		rs ;
	int		mlen = 0 ;

	if (mbp == nullptr) return SR_FAULT ;
	if (mip == nullptr) return SR_FAULT ;

	if (mbp->magic != MAILBOX_MAGIC) return SR_NOTOPEN ;

	if (mi < 0) return SR_INVALID ;

	if (mi >= mbp->msgs_total) return SR_NOTFOUND ;

	if ((rs = vecobj_get(&mbp->msgs,mi,&mp)) >= 0) {
	    *mip = *mp ;
	    mlen = mp->mlen ;
	}

	return (rs >= 0) ? mlen : rs ;
}
/* end subroutine (mailbox_msginfo) */

int mailbox_msghdradd(mailbox *mbp,int mi,cchar *k,cchar *vp,int vl) noex {
	MB_MI	*mp ;
	int		rs ;

	if (mbp == nullptr) return SR_FAULT ;
	if (k == nullptr) return SR_FAULT ;

	if (mbp->magic != MAILBOX_MAGIC) return SR_NOTOPEN ;

	if (k[0] == '\0') return SR_INVALID ;

	if ((rs = vecobj_get(&mbp->msgs,mi,&mp)) >= 0) {
	    if (! mp->f.hdradds) {
	        mp->f.hdradds = true ;
	        rs = vecstr_start(&mp->hdradds,1,0) ;
	    }
	    if (rs >= 0) {
	        mbp->f.rewrite = true ;
	        mp->f.addany = true ;
	        rs = vecstr_envadd(&mp->hdradds,k,vp,vl) ;
	    }
	}

	return rs ;
}
/* end subroutine (mailbox_msghdradd) */

int mailbox_readbegin(mailbox *mbp,MAILBOX_READ *curp,off_t foff,int rsz) noex {
	int		rs ;
	char		*p ;

	if (mbp == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (rsz <= 0) rsz = MAILBOX_READSIZE ;

	memclear(curp) ;

	if ((rs = uc_malloc(rsz,&p)) >= 0) {
	    curp->rbuf = p ;
	    curp->rsize = rsz ;
	    curp->foff = foff ;
	    curp->roff = foff ;
	}

	return rs ;
}
/* end subroutine (mailbox_readbegin) */

int mailbox_readend(mailbox *mbp,MAILBOX_READ *curp) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (mbp == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (curp->rbuf == nullptr) return SR_BADFD ;

	rs1 = uc_free(curp->rbuf) ;
	if (rs >= 0) rs = rs1 ;
	curp->rbuf = nullptr ;
	curp->rbp = nullptr ;
	curp->rsize = 0 ;
	curp->rlen = 0 ;
	curp->foff = 0 ;
	curp->roff = 0 ;

	return rs ;
}
/* end subroutine (mailbox_readend) */

int mailbox_readline(mailbox *mbp,MAILBOX_READ *curp,char *lbuf,int llen) noex {
	int		rs = SR_OK ;
	int		mfd ;
	int		i, mlen ;
	int		tlen = 0 ;
	char		*lbp = lbuf ;
	char		*bp, *lastp ;

	if (mbp == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (lbuf == nullptr) return SR_FAULT ;

	lbuf[0] = '\0' ;
	mfd = mbp->mfd ;
	while (tlen < llen) {

	    if (curp->rlen == 0) {
	        off_t	po = (curp->roff+tlen) ;
	        curp->rbp = curp->rbuf ;
	        rs = u_pread(mfd,curp->rbuf,curp->rsize,po) ;
	        if (rs >= 0) {
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
	    }

	    curp->rbp += i ;
	    tlen += i ;
	    curp->rlen -= i ;
	    if ((i > 0) && (lbp[-1] == '\n')) break ;
	} /* end while (trying to satisfy request) */

	if (rs >= 0) {
#if	CF_NULTERM
	    *lbp = '\0' ;
#endif
	    curp->roff += tlen ;
	}

	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (mailbox_readline) */


/* private subroutines */

static int mailbox_parse(mailbox *mbp) noex {
	LINER		ls, *lsp = &ls ;
	filebuf		fb ;
	const off_t	soff = 0L ;
	cint		bsize = (32 * mbp->pagesize) ;
	int		rs ;
	int		rs1 ;
	int		to = -1 ;

#if	CF_READTO
	to = mbp->to ;
#endif

/* parse out this mailbox file the hard way */

	if ((rs = filebuf_start(&fb,mbp->mfd,soff,bsize,0)) >= 0) {

	    if ((rs = liner_start(lsp,&fb,soff,to)) >= 0) {
	        int	mi = 0 ;

	        while ((rs = mailbox_parsemsg(mbp,lsp,mi)) > 0) {
	            mi += 1 ;
	        } /* end while */

	        mbp->mblen = (lsp->foff - soff) ;
	        mbp->msgs_total = mi ;

	        rs1 = liner_finish(lsp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (liner) */

	    rs1 = filebuf_finish(&fb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filebuf) */

	return (rs >= 0) ? mbp->msgs_total : rs ;
}
/* end subroutine (mailbox_parse) */

static int mailbox_parsemsg(mailbox *mbp,LINER *lsp,int mi) noex {
	MAILBOXPI	pi ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		ll = 0 ;

	if ((rs = mailboxpi_start(&pi,lsp,mi)) >= 0) {
	    mailmsgenv	me ;
	    int			vi = 0 ;
	    cchar		*lp ;

/* find message start */

	    while ((rs = liner_read(lsp,&lp)) >= 0) {
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
	        liner_done(lsp) ;
	        if (pi.f.feoh) break ;
	    } /* end while */

	    if ((rs >= 0) && mailboxpi_havemsg(&pi)) {
	        rs = mailbox_parsemsger(mbp,&me,&pi) ;
	        ll = rs ;
	    }

	    rs1 = mailboxpi_finish(&pi) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (mailboxpi) */

	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (mailbox_parsemsg) */

static int mailbox_parsemsger(mailbox *mbp,mailmsgenv *mep,
		MAILBOXPI *pip) noex {
	LINER		*lsp = pip->lsp ;
	MB_MI	msg, *msgp = &msg ;
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
	            liner_done(lsp) ;
	        }
	    }

/* read headers (ignoring envelope) */

	    while ((rs >= 0) && ((rs = liner_read(lsp,&lp)) >= 0)) {
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

	                rs = mailbox_loadmsghead(mbp,msgp,&mhv) ;

	                pip->f.fmhv = false ;
	                mailmsghdrval_finish(&mhv) ;

	            } /* end if (had a previous value outstanding) */

	            if (msgp->hoff < 0)
	                msgp->hoff = lsp->poff ;

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
	                rs = mailbox_loadmsghead(mbp,msgp,&mhv) ;
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
	        liner_done(lsp) ;

	        if (pip->f.feoh) break ;
	        if (rs < 0) break ;
	    } /* end while (reading lines) */

/* load last header */

	    if (rs >= 0) {

	        if (pip->f.fmhv) {
	            if (rs >= 0) {
	                rs = mailbox_loadmsghead(mbp,msgp,&mhv) ;
	            }
	            pip->f.fmhv = false ;
	            mailmsghdrval_finish(&mhv) ;
	        } /* end if */

	        if (msgp->hoff < 0)
	            msgp->hoff = lsp->poff ;

	        if (msgp->hlen < 0)
	            msgp->hlen = (lsp->poff - msgp->hoff) ;

	        if (pip->f.feoh) {
	            msgp->boff = lsp->foff ;
	        }

	    } /* end if (loading last header) */

/* handle reading the body */

	    if (rs >= 0) {

	        if (mbp->f.useclen && msgp->hdrval.clen) {
	            clen = msgp->clen ;
	            rs = liner_seek(lsp,msgp->clen) ;
	        } else {
	            int		linemax = INT_MAX ;
	            if (mbp->f.useclines && 
	                msgp->hdrval.clines && (msgp->clines >= 0))
	                linemax = msgp->clines ;
	            pip->f.fbol = true ;
	            clines = 0 ;
	            while ((rs >= 0) && (clines < linemax) && 
	                ((rs = liner_read(lsp,&lp)) >= 0)) {

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
	                liner_done(lsp) ;

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
	        rs = vecobj_add(&mbp->msgs,msgp) ;
	        if (rs >= 0) pip->f.fmsg = false ;
	    } /* end if (insertion) */

	    rs1 = msginfo_finish(msgp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (msginfo) */

	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (mailbox_parsemsger) */

static int mailbox_loadmsghead(mailbox *mbp,MB_MI *msgp,
		mailmsghdrval *mhvp) noex {
	int		rs ;
	int		vl ;
	int		hi ;
	int		v = -1 ;
	cchar		*vp ;

	if (mbp == nullptr) return SR_FAULT ;

	rs = mailmsghdrval_get(mhvp,&vp,&vl) ;
	hi = rs ;
	if (rs >= 0) {
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

	return (rs >= 0) ? hi : rs ;
}
/* end subroutine (mailbox_loadmsghead) */

static int mailbox_msgfins(mailbox *mbp) noex {
	MB_MI		*mp ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ; /* used afterwards */
	for (i = 0 ; vecobj_get(&mbp->msgs,i,&mp) >= 0 ; i += 1) {
	    if (mp == nullptr) continue ;
	    rs1 = msginfo_finish(mp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end for */

	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mailbox_msgfins) */

static int mailbox_rewrite(mailbox *mbp) noex {
	cint		dlen = MAXPATHLEN ;
	int		rs ;
	char		dbuf[MAXPATHLEN + 1] ;

	if ((rs = tmpmailboxes(dbuf,dlen)) >= 0) {
	    cchar	*mb = "mbXXXXXXXXXXXX" ;
	    char	template[MAXPATHLEN + 1] ;
	    if ((rs = mkpath2(template,dbuf,mb)) >= 0) {
		sigblocker	ss ;
	        if ((rs = sigblocker_start(&ss,sigblockers)) >= 0) {
	            const mode_t	om = 0600 ;
	            cint	of = (O_RDWR | O_CREAT) ;
	            char		tbuf[MAXPATHLEN + 1] ;

	            if ((rs = opentmpfile(template,of,om,tbuf)) >= 0) {
	                cint	tfd = rs ;

	                rs = mailbox_rewriter(mbp,tfd) ;

	                u_close(tfd) ;
	                u_unlink(tbuf) ;
	            } /* end if (open tmp-file) */

	            sigblocker_finish(&ss) ;
	        } /* end if (sigblocker) */
	    } /* end if (mkpath) */
	} /* end if (tmpmailboxes) */

	return rs ;
}
/* end subroutine (mailbox_rewrite) */

static int mailbox_rewriter(mailbox *mbp,int tfd) noex {
	MSGCOPY		mc{} ;
	MB_MI		*mip ;
	filebuf		fb, *fbp = &fb ;
	off_t		mbchange = -1 ;
	cint		pagesize = mbp->pagesize ;
	int		rs ;
	int		bsize ;
	int		rs1 ;
	int		mfd = mbp->mfd ;
	int		mi ;
	int		bl ;
	int		elen = 0 ; /* GCC false complaint */
	int		wlen = 0 ;
	int		f_del ;
	int		f_copy = false ;
	int		f ;
	char		*bp ;

	bl = pagesize ;
	if ((rs = uc_valloc(bl,&bp)) >= 0) {

	    mc.fbp = fbp ;
	    mc.bp = bp ;
	    mc.bl = bl ;
	    mc.moff = 0 ;

	    bsize = min(iceil(mbp->mblen,pagesize),(16*pagesize)) ;

	    if ((rs = filebuf_start(fbp,tfd,0L,bsize,0)) >= 0) {

	        for (mi = 0 ; mi < mbp->msgs_total ; mi += 1) {

	            rs = vecobj_get(&mbp->msgs,mi,&mip) ;
	            if (rs < 0) break ;

	            f_del  = mip->cmd.msgdel ;
	            f = f_del || mip->f.addany ;
	            if (f) {
	                if (mbchange < 0) {
	                    mbchange = mip->moff ;
	                    f_copy = true ;
	                }
	            }

	            if (f_copy) {
	                if (! f_del) {
	                    rs = mailbox_msgcopy(mbp,&mc,mip) ;
	                    wlen += rs ;
	                }
	            } /* end if */

	            if (rs < 0) break ;
	        } /* end for */

	        if (rs >= 0) {

	            if (mc.moff != mbp->mblen) {
	                u_seek(mfd,mbp->mblen,SEEK_SET) ;
	                mc.moff = mbp->mblen ;
	            }
	            rs = filebuf_writefd(fbp,bp,bl,mfd,-1) ;
	            elen = rs ;
	            wlen += rs ;

	        } /* end if (finishing off) */

	        filebuf_finish(fbp) ;
	    } /* end if (filebuf) */

	    if (rs >= 0) {

/* extend the mailbox file if necessary (rarely happens?) */

	        if ((mbp->mblen + elen) < (mbchange + wlen)) {
	            rs1 = (mbchange + wlen) - (mbp->mblen + elen) ;
	            rs = writeblanks(mfd,rs1) ;
	        } /* end if */

/* copy adjusted data back to the mailbox file */

	        if ((rs >= 0) && (wlen > 0)) {
	            u_seek(mfd,mbchange,SEEK_SET) ;
	            u_seek(tfd,0L,SEEK_SET) ;
	            rs = uc_writedesc(mfd,tfd,-1) ;
	        }

/* truncate mailbox file if necessary */

	        if ((rs >= 0) && (mbp->mblen > (mbchange + wlen))) {
	            rs = uc_ftruncate(mfd,(mbchange+wlen)) ;
	        }

	    } /* end if */

	    uc_free(bp) ;
	} /* end if (memory-allocation) */

	return rs ;
}
/* end subroutine (mailbox_rewriter) */

static int mailbox_msgcopy(mailbox *mbp,MSGCOPY *mcp,MB_MI *mip) noex {
	int		rs = SR_OK ;
	int		mfd = mbp->mfd ;
	int		wlen = 0 ;

	if (mcp->moff != mip->moff) {
	    rs = u_seek(mfd,mip->moff,SEEK_SET) ;
	    mcp->moff = mip->moff ;
	}

	if (rs >= 0) {
	    if (mip->f.addany) {
	        rs = mailbox_msgcopyadd(mbp,mcp,mip) ;
	        wlen += rs ;
	    } else {
	        rs = filebuf_writefd(mcp->fbp,mcp->bp,mcp->bl,mfd,mip->mlen) ;
	        wlen += rs ;
	    }
	    mcp->moff += wlen ;
	} /* end if (ok) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mailbox_msgcopy) */

static int mailbox_msgcopyadd(mailbox *mbp,MSGCOPY *mcp,MB_MI *mip) noex {
	cint		mfd = mbp->mfd ;
	int		rs = SR_OK ;
	int		ehlen ;
	int		i ;
	int		wlen = 0 ;
	cchar	*sp ;

#if	CF_DEBUGS
	debugprintf("mailbox_msgcopyadd: mi=%u\n",mip->msgi) ;
#endif

	ehlen = ((mip->hoff + mip->hlen) - mip->moff) ;
	if (rs >= 0) {
	    rs = filebuf_writefd(mcp->fbp,mcp->bp,mcp->bl,mfd,ehlen) ;
	    wlen += rs ;
	}

#if	CF_DEBUGS
	debugprintf("mailbox_msgcopyadd: hdr-add\n") ;
#endif

	if ((rs >= 0) && mip->f.addany && mip->f.hdradds) {
	    for (i = 0 ; vecstr_get(&mip->hdradds,i,&sp) >= 0 ; i += 1) {
	        if (sp != nullptr) {
	            rs = filebuf_writehdr(mcp->fbp,sp,-1) ;
	            wlen += rs ;
		}
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if */

#if	CF_DEBUGS
	debugprintf("mailbox_msgcopyadd: end-of-header and body\n") ;
#endif

	if (rs >= 0) {
	    rs = filebuf_writefd(mcp->fbp,mcp->bp,mcp->bl,mfd,(mip->blen+1)) ;
	    wlen += rs ;
	}

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

static int msginfo_setenv(MB_MI *msgp,mailmsgenv *mep) noex {
	int		rs = SR_OK ;
	msgp->f.env = true ;
	msgp->f.senv = mep->f.start ;
	return rs ;
}
/* end subroutine (msginfo_setenv) */

static int liner_start(LINER *lsp,filebuf *fbp,off_t foff,int to) noex {
	int		rs = SR_FAULT ;
	if (lsp && dbp) {
	    char	*lp{} ;
	    if ((rs = malloc_ml(&lp)) >= 0) {
		lp[0] = '\0' ;
	        lsp->llen = rs ;
	        lsp->lbuf = lp ;
	        lsp->poff = 0 ;
	        lsp->foff = foff ;
	        lsp->fbp = fbp ;
	        lsp->to = to ;
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (liner_start) */

static int liner_finish(LINER *lsp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (lsp) {
	    rs = SR_OK ;
	    lsp->llen = 0 ;
	    lsp->lbuf[0] = '\0' ;
	    {
	        rs1 = uc_free(lsp->lbuf) ;
		if (rs >= 0) rs = rs1 ;
	        lsp->lbuf = nullptr ;
	    {
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (liner_finish) */

static int liner_read(LINER *lsp,cchar **lpp) noex {
	filebuf		*fbp = lsp->fbp ;
	int		rs = SR_OK ;
	int		len = 0 ;
	if (lsp->llen < 0) {
	    cint	ll = lsp->llen ;
	    cchar	*lp = lsp->lbuf ;
	    lsp->poff = lsp->foff ;
	    if ((rs = filebuf_readln(fbp,lp,ll,lsp->to)) >= 0) {
		len = rs ;
	        lsp->llen = rs ;
	        lsp->foff += lsp->llen ;
	    }
	} /* end if (needed a new line) */
	if (lpp) {
	    *lpp = (rs >= 0) ? lsp->lbuf : nullptr ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (liner_read) */

static int liner_done(LINER *lsp) noex {
	int		rs = SR_FAULT ;
	if (lsp) {
	    rs = SR_OK ;
	    lsp->poff = lsp->foff ;
	    lsp->llen = -1 ;
	    lsp->lbuf[0] = '\0' ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (liner_done) */

static int liner_seek(LINER *lsp,int inc) noex {
	int		rs = SR_OK ;
	lsp->poff = lsp->foff ;
	if (inc > 0) {
	    lsp->llen = -1 ;
	    lsp->lbuf[0] = '\0' ;
	    lsp->poff += inc ;
	    lsp->foff += inc ;
	    rs = filebuf_adv(lsp->fbp,inc) ;
	} /* end if */
	return rs ;
}
/* end subroutine (liner_seek) */

static int mailboxpi_finish(MAILBOXPI *pip) noex {
	if (pip == nullptr) return SR_FAULT ;
	return SR_OK ;
}
/* end subroutine (mailboxpi_finish) */

static int mailboxpi_havemsg(MAILBOXPI *pip) noex {
	return (pip->f.fenv || pip->f.fhdr || pip->f.feoh) ;
}
/* end subroutine (mailboxpi_havemsg) */

static int mailboxpi_start(MAILBOXPI *pip,LINER *lsp,int mi) noex {
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

#if	CF_DEBUGS
static int debuglinelen(cchar *lp,int ll,int ml) noex {
	int		rl = strnlen(lp,ll) ;
	if (rl > ml) {
	    rl = ml ;
	}
	if ((rl > 0) && (lp[rl-1] == '\n')) {
	    rl -= 1 ;
	}
	return rl ;
}
/* end subroutine (debuglinelen) */
#endif /* CF_DEBUGS */



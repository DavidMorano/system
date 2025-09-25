/* mfs-c SUPPORT (MFS CLient code) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* MFS Client code */
/* object to interact with the MFS server */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 2000-12-18, David A­D­ Morano
	This object module was first written.

	= 2011-01-25, David A­D­ Morano
	I added the capability to also send the 'mark', 'report',
	and 'exit' commands to the server.  Previously these were
	not implemented here.

	= 2017-08-10, David A­D­ Morano
	This subroutine was borrowed to code MFSERVE.

*/

/* Copyright © 2000,2011,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	mfsc

	Description:
	This module obkect is a client manager for client requests.
	This is client-side code that interacts with the server
	(MFSERVE).

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<estrings.h>
#include	<vechand.h>
#include	<storebuf.h>
#include	<sockaddress.h>
#include	<envmgr.h>
#include	<spawnproc.h>
#include	<ctdec.h>
#include	<listenspec.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"mfsc.h"
#include	"mfsmsg.h"
#include	"pcsnsreq.h"


/* local defines */

#define	MFSC_VARPR	"LOCAL"
#define	MFSC_FACNAME	"mfserve"
#define	MFSC_PIDNAME	"pid"
#define	MFSC_REQNAME	"req"
#define	MFSC_DMODE	0777

#define	VARPREXTRA	"EXTRA" ;
#define	VARMFSQUIET	"MFSERVE_QUIET"
#define	VARMFSPR	"MFSERVE_PROGRAMROOT"

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif

#ifndef	MSGBUFLEN
#define	MSGBUFLEN	2048
#endif

#define	OPTBUFLEN	(DIGBUFLEN + 4)

#define	TO_UPDATE	60
#define	TO_RUN		(5 * 60)
#define	TO_RECVMSG	5


/* external subroutines */

#if	CF_DEBUGS
extern int	debugprintf(cchar *,...) ;
extern int	debugprinthexblock(cchar *,int,const void *,int) ;
#endif


/* external variables */


/* local structures */


/* forward references */

static int	mfsc_setbegin(MFSC *,cchar *) noex ;
static int	mfsc_setend(MFSC *) noex ;
static int	mfsc_srvdname(MFSC *,char *) noex ;
static int	mfsc_srvfname(MFSC *,cchar *) noex ;
static int	mfsc_bind(MFSC *,int,cchar *) noex ;
static int	mfsc_bufbegin(MFSC *) noex ;
static int	mfsc_bufend(MFSC *) noex ;
static int	mfsc_connect(MFSC *) noex ;
static int	mfsc_istatus(MFSC *,MFSC_STATUS *) noex ;
static int	mfsc_listenerfmt(MFSC *,char *,int,int,MFSMSG_LISTENER *) noex ;

#ifdef	COMMENT
static int	mfsc_spawn(MFSC *) noex ;
static int	mfsc_envload(MFSC *,ENVMGR *) noex ;
#endif

static int	mksrvdname(char *,cchar *,cchar *,cchar *) noex ;


/* local variables */

#ifdef	COMMENT
static cpcchar		prbins[] = {
	"bin",
	"sbin",
	NULL
} ;
#endif /* COMMENT */


/* exported variables */

mfsc_obj	mfsc_modinfo = {
	"mfsc",
	sizeof(mfsc)
} ;


/* exported subroutines */

int mfsc_open(MFSC *op,cchar *pr,int to) noex {
	int		rs ;

	if (op == NULL) return SR_FAULT ;
	if (pr == NULL) return SR_FAULT ;
	if (pr[0] == '\0') return SR_INVALID ;

	if (to < 1) to = 1 ;

#if	CF_DEBUGS
	debugprintf("mfsc_open: ent pr=%s\n",pr) ;
#endif

	memclear(op) ;
	op->fd = -1 ;
	op->to = to ;
	op->pid = getpid() ;

	if ((rs = mfsc_setbegin(op,pr)) >= 0) {
	    if ((rs = mfsc_connect(op)) > 0) {
	        if ((rs = mfsc_bufbegin(op)) >= 0) {
	            op->fl.srv = TRUE ;
	            rs = 1 ;
	            op->magic = MFSC_MAGIC ;
	        }
	    } /* end if (mfsc_connect) */
	    if ((rs < 0) || (! op->fl.srv)) {
	        mfsc_setend(op) ;
	    }
	} /* end if (mfsc-set) */

#if	CF_DEBUGS
	debugprintf("mfsc_open: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (mfsc_open) */

int mfsc_close(MFSC *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != MFSC_MAGIC) return SR_NOTOPEN ;

	rs1 = mfsc_bufend(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = mfsc_setend(op) ;
	if (rs >= 0) rs = rs1 ;

#if	CF_DEBUGS
	debugprintf("mfsc_close: ret rs=%d\n",rs) ;
#endif

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (mfsc_close) */

int mfsc_getstat(MFSC *op,MFSC_STATUS *statp) noex {
	int		rs ;

#if	CF_DEBUGS
	debugprintf("mfsc_status: ent\n") ;
#endif

	if (op == NULL) return SR_FAULT ;

	if (op->magic != MFSC_MAGIC) return SR_NOTOPEN ;

#if	CF_DEBUGS
	debugprintf("mfsc_status: con\n") ;
#endif

	rs = mfsc_istatus(op,statp) ;

#if	CF_DEBUGS
	debugprintf("mfsc_status: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (mfsc_getstat) */

int mfsc_help(MFSC *op,char *rbuf,int rlen,int idx) noex {
	int		rs = SR_OK ;
	int		rl = 0 ;

#if	CF_DEBUGS
	debugprintf("mfsc_help: ent\n") ;
#endif

	if (op == NULL) return SR_FAULT ;
	if (rbuf == NULL) return SR_FAULT ;

	if (op->magic != MFSC_MAGIC) return SR_NOTOPEN ;

	if (op->fl.srv) {
	    struct mfsmsg_gethelp	mreq ;
	    struct mfsmsg_help		mres ;
	    cint		to = op->to ;
	    cint		mlen = op->mlen ;
	    char		*mbuf = op->mbuf ;
	    mreq.tag = op->pid ;
	    mreq.idx = (uchar) idx ;
	    if ((rs = mfsmsg_gethelp(&mreq,0,mbuf,mlen)) >= 0) {
	        cint	fd = op->fd ;
	        if ((rs = u_send(fd,mbuf,rs,0)) >= 0) {
	            cint	mf = 0 ;
	            cint	ro = FM_TIMED ;
	            if ((rs = uc_recve(fd,mbuf,mlen,mf,to,ro)) >= 0) {
	                if ((rs = mfsmsg_help(&mres,1,mbuf,rs)) >= 0) {
	                    if (mres.rc == mfsmsgrc_ok) {
	                        cchar	*rp = mres.val ;
	                        rl = mres.vl ;
	                        rs = snwcpy(rbuf,rlen,rp,rl) ;
	                    } else if (mres.rc == mfsmsgrc_notfound) {
	                        rl = 0 ;
	                    } else {
	                        rs = SR_BADMSG ;
	                    }
	                }
	            } else if (isBadRecv(rs)) {
	                op->fl.srv = FALSE ;
	                rs = SR_OK ;
	            } /* end if (uc_recve) */
	        } else if (isBadSend(rs)) {
	            op->fl.srv = FALSE ;
	            rs = SR_OK ;
	        } /* end if (u_send) */
	    } /* end if (mfsmsg_gethelp) */
	    if (rs < 0) op->fl.srv = FALSE ;
	} /* end if (servicing) */

#if	CF_DEBUGS
	debugprintf("mfsc_help: ret rs=%d rl=%u\n",rs,rl) ;
#endif

	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mfsc_help) */

int mfsc_getval(MFSC *op,char *rbuf,int rlen,cchar *un,int w) noex {
	int		rs = SR_OK ;
	int		rl = 0 ;

#if	CF_DEBUGS
	debugprintf("mfsc_getval: ent un=%s w=%u\n",un,w) ;
#endif

	if (op == NULL) return SR_FAULT ;
	if (rbuf == NULL) return SR_FAULT ;
	if (un == NULL) return SR_FAULT ;

	if (un[0] == '\0') return SR_INVALID ;

	if (op->magic != MFSC_MAGIC) return SR_NOTOPEN ;

	if (op->fl.srv) {
	    struct mfsmsg_getval	mreq ;
	    struct mfsmsg_val		mres ;
	    cint		to = op->to ;
	    cint		mlen = op->mlen ;
	    char		*mbuf = op->mbuf ;
	    mreq.tag = op->pid ;
	    mreq.w = (uchar) w ;
	    strwcpy(mreq.key,un,MFSMSG_KEYLEN) ;
	    if ((rs = mfsmsg_getval(&mreq,0,mbuf,mlen)) >= 0) {
	        cint	fd = op->fd ;
	        if ((rs = u_send(fd,mbuf,rs,0)) >= 0) {
	            cint	mf = 0 ;
	            cint	ro = FM_TIMED ;
	            if ((rs = uc_recve(fd,mbuf,mlen,mf,to,ro)) >= 0) {
	                if ((rs = mfsmsg_val(&mres,1,mbuf,rs)) >= 0) {
	                    if (mres.rc == mfsmsgrc_ok) {
	                        cchar	*rp = mres.val ;
	                        rl = mres.vl ;
	                        rs = snwcpy(rbuf,rlen,rp,rl) ;
	                    } else if (mres.rc == mfsmsgrc_notfound) {
	                        rl = 0 ;
	                    } else {
	                        rs = SR_BADMSG ;
	                    }
	                }
	            } else if (isBadRecv(rs)) {
	                op->fl.srv = FALSE ;
	                rs = SR_OK ;
	            } /* end if (uc_recve) */
	        } else if (isBadSend(rs)) {
	            op->fl.srv = FALSE ;
	            rs = SR_OK ;
	        } /* end if (u_send) */
	    } /* end if (mfsmsg_getval) */
	    if (rs < 0) op->fl.srv = FALSE ;
	} /* end if (servicing) */

#if	CF_DEBUGS
	debugprintf("mfsc_getval: ret rs=%d rl=%u\n",rs,rl) ;
#endif

	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mfsc_getval) */

int mfsc_mark(MFSC *op) noex {
	int		rs = SR_OK ;
	int		rl = 0 ;

#if	CF_DEBUGS
	debugprintf("mfsc_mark: ent\n") ;
#endif

	if (op == NULL) return SR_FAULT ;

	if (op->magic != MFSC_MAGIC) return SR_NOTOPEN ;

	if (op->fl.srv) {
	    struct mfsmsg_mark	mreq ;
	    struct mfsmsg_ack	mres ;
	    cint		to = op->to ;
	    cint		mlen = op->mlen ;
	    char		*mbuf = op->mbuf ;
	    mreq.tag = op->pid ;
	    if ((rs = mfsmsg_mark(&mreq,0,mbuf,mlen)) >= 0) {
	        cint	fd = op->fd ;
	        if ((rs = u_send(fd,mbuf,rs,0)) >= 0) {
	            cint	mf = 0 ;
	            cint	ro = FM_TIMED ;
	            if ((rs = uc_recve(fd,mbuf,mlen,mf,to,ro)) >= 0) {
	                if ((rs = mfsmsg_ack(&mres,1,mbuf,rs)) >= 0) {
	                    if (mres.rc == mfsmsgrc_ok) {
	                        rl = 1 ;
	                    } else {
	                        rs = SR_BADMSG ;
	                    }
	                }
	            } else if (isBadRecv(rs)) {
	                op->fl.srv = FALSE ;
	                rs = SR_OK ;
	            } /* end if (uc_recve) */
	        } else if (isBadSend(rs)) {
	            op->fl.srv = FALSE ;
	            rs = SR_OK ;
	        } /* end if (u_send) */
	    } /* end if (mfsmsg_mark) */
	    if (rs < 0) op->fl.srv = FALSE ;
	} /* end if (servicing) */

#if	CF_DEBUGS
	debugprintf("mfsc_mark: ret rs=%d\n",rs) ;
#endif

	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mfsc_mark) */

int mfsc_exit(MFSC *op,cchar *reason) noex {
	int		rs = SR_OK ;
	int		rl = 0 ;

#if	CF_DEBUGS
	debugprintf("mfsc_exit: ent\n") ;
#endif

	if (op == NULL) return SR_FAULT ;

	if (op->magic != MFSC_MAGIC) return SR_NOTOPEN ;

	if (op->fl.srv) {
	    struct mfsmsg_exit	mreq ;
	    struct mfsmsg_ack	mres ;
	    cint		to = op->to ;
	    cint		mlen = op->mlen ;
	    char		*mbuf = op->mbuf ;
	    mreq.tag = op->pid ;
	    mreq.reason[0] = '\0' ;
	    if (reason != NULL) {
	        strwcpy(mreq.reason,reason,REALNAMELEN) ;
	    }
	    if ((rs = mfsmsg_exit(&mreq,0,mbuf,mlen)) >= 0) {
	        cint	fd = op->fd ;
	        if ((rs = u_send(fd,mbuf,rs,0)) >= 0) {
	            cint	mf = 0 ;
	            cint	ro = FM_TIMED ;
	            if ((rs = uc_recve(fd,mbuf,mlen,mf,to,ro)) >= 0) {
	                if ((rs = mfsmsg_ack(&mres,1,mbuf,rs)) >= 0) {
	                    if (mres.rc == mfsmsgrc_ok) {
	                        rl = 1 ;
	                    } else {
	                        rs = SR_BADMSG ;
	                    }
	                }
	            } else if (isBadRecv(rs)) {
	                op->fl.srv = FALSE ;
	                rs = SR_OK ;
	            } /* end if (uc_recve) */
	        } else if (isBadSend(rs)) {
	            op->fl.srv = FALSE ;
	            rs = SR_OK ;
	        } /* end if (u_send) */
	    } /* end if (mfsmsg_exit) */
	    if (rs < 0) op->fl.srv = FALSE ;
	} /* end if (servicing) */

#if	CF_DEBUGS
	debugprintf("mfsc_exit: ret rs=%d\n",rs) ;
#endif

	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mfsc_exit) */

int mfsc_listener(MFSC *op,char *rbuf,int rlen,int idx) noex {
	int		rs = SR_OK ;
	int		rl = 0 ;

#if	CF_DEBUGS
	debugprintf("mfsc_listener: ent\n") ;
#endif

	if (op == NULL) return SR_FAULT ;
	if (rbuf == NULL) return SR_FAULT ;

	if (op->magic != MFSC_MAGIC) return SR_NOTOPEN ;

	if (op->fl.srv) {
	    struct mfsmsg_getlistener	mreq ;
	    struct mfsmsg_listener	mres ;
	    cint		to = op->to ;
	    cint		mlen = op->mlen ;
	    char		*mbuf = op->mbuf ;
	    mreq.tag = op->pid ;
	    mreq.idx = (uchar) idx ;
	    if ((rs = mfsmsg_getlistener(&mreq,0,mbuf,mlen)) >= 0) {
	        cint	fd = op->fd ;
	        if ((rs = u_send(fd,mbuf,rs,0)) >= 0) {
	            cint	mf = 0 ;
	            cint	ro = FM_TIMED ;
	            if ((rs = uc_recve(fd,mbuf,mlen,mf,to,ro)) >= 0) {
	                if ((rs = mfsmsg_listener(&mres,1,mbuf,rs)) >= 0) {
	                    if (mres.rc == mfsmsgrc_ok) {
	                        rs = mfsc_listenerfmt(op,rbuf,rlen,idx,&mres) ;
	                        rl = rs ;
	                    } else if (mres.rc == mfsmsgrc_notfound) {
	                        rl = 0 ;
	                    } else {
	                        rs = SR_BADMSG ;
	                    }
	                }
	            } else if (isBadRecv(rs)) {
	                op->fl.srv = FALSE ;
	                rs = SR_OK ;
	            } /* end if (uc_recve) */
	        } else if (isBadSend(rs)) {
	            op->fl.srv = FALSE ;
	            rs = SR_OK ;
	        } /* end if (u_send) */
	    } /* end if (mfsmsg_gethelp) */
	    if (rs < 0) op->fl.srv = FALSE ;
	} /* end if (servicing) */

#if	CF_DEBUGS
	debugprintf("mfsc_listener: ret rs=%d rl=%u\n",rs,rl) ;
#endif

	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mfsc_listener) */

int mfsc_getname(MFSC *op,char *rbuf,int rlen,cchar *un) noex {
	cint	w = pcsnsreq_pcsname ;
	return mfsc_getval(op,rbuf,rlen,un,w) ;
}
/* end subroutine (mfsc_getname) */


/* local subroutines */

static int mfsc_setbegin(MFSC *op,cchar *pr) noex {
	int		rs ;
	int		f = FALSE ;
	cchar		*cp ;

#if	CF_DEBUGS
	debugprintf("mfsc_setbegin: ent\n") ;
#endif

	if ((rs = uc_mallocstrw(pr,-1,&cp)) >= 0) {
	    char	rbuf[MAXPATHLEN+1] ;
	    op->pr = cp ;
	    if ((rs = mfsc_srvdname(op,rbuf)) > 0) {
	        if ((rs = mfsc_srvfname(op,rbuf)) > 0) {
	            if ((rs = mfsc_bind(op,TRUE,rbuf)) >= 0) {
	                f = TRUE ;
	            }
	            if ((rs < 0) && (op->srvfname != NULL)) {
	                uc_free(op->srvfname) ;
	                op->srvfname = NULL ;
	            }
	        } /* end if (mfsc_srvfname) */
	    } /* end if (mfsc_srvdname) */
	    if (((rs < 0) || (!f)) && (op->pr != NULL)) {
	        uc_free(op->pr) ;
	        op->pr = NULL ;
	    }
	} /* end if (m-a) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mfsc_setbegin) */

static int mfsc_setend(MFSC *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	rs1 = mfsc_bufend(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = mfsc_bind(op,FALSE,NULL) ;
	if (rs >= 0) rs = rs1 ;

	if (op->srvfname != NULL) {
	    rs1 = uc_free(op->srvfname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->srvfname = NULL ;
	}

	if (op->pr != NULL) {
	    rs1 = uc_free(op->pr) ;
	    if (rs >= 0) rs = rs1 ;
	    op->pr = NULL ;
	}

	return rs ;
}
/* end subroutine (mfsc_setend) */

static int mfsc_srvdname(MFSC *op,char *rbuf) noex {
	int		rs ;
	int		rl = 0 ;
	cchar		*td = TMPDNAME ;
	cchar		*fn = MFSC_FACNAME ;

	if ((rs = mksrvdname(rbuf,td,op->pr,fn)) >= 0) {
	    USTAT	sb ;
	    if ((rs = u_stat(rbuf,&sb)) >= 0) {
	        if (S_ISDIR(sb.st_mode)) {
	            cint	am = (W_OK|W_OK|X_OK) ;
	            if ((rs = perm(rbuf,-1,-1,NULL,am)) >= 0) {
	                rl = 1 ;
	            } else if (isNotAccess(rs)) {
	                rs = SR_OK ;
	            }
	        }
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    }
	}
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mfsc_srvdname) */

static int mfsc_srvfname(MFSC *op,cchar *srvdname) noex {
	int		rs ;
	int		rl = 0 ;
	cchar	*reqname = MFSC_REQNAME ;
	char		srvfname[MAXPATHLEN + 1] ;

	if ((rs = mkpath2(srvfname,srvdname,reqname)) >= 0) {
	    USTAT	sb ;
	    rl = rs ;
	    if ((rs = u_stat(srvfname,&sb)) >= 0) {
	        if (S_ISSOCK(sb.st_mode)) {
	            cint	am = (R_OK|W_OK) ;
	            if ((rs = perm(srvfname,-1,-1,NULL,am)) >= 0) {
	                cchar	*cp ;
	                if ((rs = uc_mallocstrw(srvfname,rl,&cp)) >= 0) {
	                    op->srvfname = cp ;
	                }
	            } else if (isNotAccess(rs)) {
	                rl = 0 ;
	                rs = SR_OK ;
	            }
	        } else {
	            rl = 0 ;
	        }
	    } else if (isNotPresent(rs)) {
	        rl = 0 ;
	        rs = SR_OK ;
	    }
	} /* end if (srvfname) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mfsc_srvfname) */

static int mfsc_bind(MFSC *op,int f,cchar *srvdname) noex {
	int		rs = SR_OK ;
	int		f_err = FALSE ;

#if	CF_DEBUGS
	debugprintf("mfsc_bind: ent f=%u\n",f) ;
#endif

	if (f) {
	    cchar	*tn = "clientXXXXXXXX" ;
	    char	tpat[MAXPATHLEN + 1] ;

	    if ((rs = mkpath2(tpat,srvdname,tn)) >= 0) {
	        cmode	om = (S_IFSOCK | 0666) ;
	        cint	of = (O_RDWR | O_CLOEXEC | O_MINMODE) ;
	        char		fname[MAXPATHLEN + 1] ;
	        if ((rs = opentmpusd(tpat,of,om,fname)) >= 0) {
	            cchar	*cp ;
	            op->fd = rs ;
	            u_chmod(fname,om) ;
	            uc_closeonexec(op->fd,TRUE) ;
	            if ((rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
	                op->srcfname = cp ;
	            } else {
	                f_err = TRUE ;
	            }
	        } /* end if (opentmpusd) */
	    } /* end if (mkpath) */

	} /* end if (bind-on) */

	if ((! f) || f_err) {

	    if (op->fd >= 0) {
	        u_close(op->fd) ;
	        op->fd = -1 ;
	    }
	    if (op->srcfname != NULL) {
	        if (op->srcfname[0] != '\0') {
	            u_unlink(op->srcfname) ;
	        }
	        uc_free(op->srcfname) ;
	        op->srcfname = NULL ;
	    }

	} /* end if (bind-off) */

#if	CF_DEBUGS
	debugprintf("mfsc_bind: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (mfsc_bind) */

static int mfsc_bufbegin(MFSC *op) noex {
	cint	blen = MSGBUFLEN ;
	int		rs ;
	char		*bp ;
	if ((rs = uc_malloc((blen+1),&bp)) >= 0) {
	    op->mbuf = bp ;
	    op->mlen = blen ;
	}
	return rs ;
}
/* end subroutine (mfsc_bufbegin) */

static int mfsc_bufend(MFSC *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->mbuf != NULL) {
	    rs1 = uc_free(op->mbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    op->mbuf = NULL ;
	    op->mlen = 0 ;
	}
	return rs ;
}
/* end subroutine (mfsc_bufend) */

static int mfsc_connect(MFSC *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = FALSE ;

#if	CF_DEBUGS
	debugprintf("mfsc_connect: srvfname=%s\n",op->srvfname) ;
#endif

	if (op->srvfname != NULL) {
	    SOCKADDRESS	sa ;
	    cint	af = AF_UNIX ;
	    cchar	*sfn = op->srvfname ;
	    if ((rs = sockaddress_start(&sa,af,sfn,0,0)) >= 0) {
	        SOCKADDR	*sap = (SOCKADDR *) &sa ;
	        cint	sal = rs ;
	        cint	to = op->to ;
	        if ((rs = uc_connecte(op->fd,sap,sal,to)) >= 0) {
	            f = TRUE ;
	        } else if (isFailConn(rs)) {
	            rs = SR_OK ;
	        }
	        rs1 = sockaddress_finish(&sa) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sockaddress) */
	} /* end if (non-null) */

#if	CF_DEBUGS
	debugprintf("mfsc_connect: ret rs=%d f=%u\n",rs,f) ;
#endif

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mfsc_connect) */

static int mfsc_istatus(MFSC *op,MFSC_STATUS *statp) noex {
	int		rs = SR_OK ;
	int		rc = 0 ;

#if	CF_DEBUGS
	debugprintf("mfsc_istatus: ent\n") ;
#endif

	if (statp != NULL) {
	    memclear(statp) ;
	}

	if (op->fl.srv) {
	    struct mfsmsg_getstatus	mreq ;
	    struct mfsmsg_status	mres ;
	    cint		to = op->to ;
	    cint		mlen = op->mlen ;
	    char		*mbuf = op->mbuf ;
	    mreq.tag = op->pid ;
	    if ((rs = mfsmsg_getstatus(&mreq,0,mbuf,mlen)) >= 0) {
	        cint	fd = op->fd ;
	        if ((rs = u_send(fd,mbuf,rs,0)) >= 0) {
	            cint	mf = 0 ;
	            cint	ro = FM_TIMED ;
	            if ((rs = uc_recve(fd,mbuf,mlen,mf,to,ro)) >= 0) {
	                if ((rs = mfsmsg_status(&mres,1,mbuf,rs)) >= 0) {
#if	CF_DEBUGS
	                    debugprintf("mfsc_istatus: "
	                        "mfsmsg_status() rs=%d rc=%u\n",rs,mres.rc) ;
#endif
	                    if (mres.rc == mfsmsgrc_ok) {
	                        rc = 1 ;
	                        if (statp != NULL) {
	                            statp->pid = mres.pid ;
	                            statp->queries = mres.queries ;
	                        }
	                    } else if (mres.rc == mfsmsgrc_notavail) {
	                        rc = 0 ;
	                    } else {
	                        rs = SR_BADMSG ;
	                    }
	                }
	            } else if (isBadRecv(rs)) {
	                op->fl.srv = FALSE ;
	                rs = SR_OK ;
	            } /* end if (uc_recve) */
#if	CF_DEBUGS
	            debugprintf("mfsc_istatus: recv-out rs=%d\n",rs) ;
#endif
	        } else if (isBadSend(rs)) {
	            op->fl.srv = FALSE ;
	            rs = SR_OK ;
	        } /* end if (u_send) */
#if	CF_DEBUGS
	        debugprintf("mfsc_istatus: send-out rs=%d\n",rs) ;
#endif
	    } /* end if (mfsmsg_getstatus) */
	    if (rs < 0) op->fl.srv = FALSE ;
	} /* end if (servicing) */

#if	CF_DEBUGS
	debugprintf("mfsc_istatus: ret rs=%d pid=%d\n",rs,rc) ;
#endif

	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (mfsc_istatus) */

static int mfsc_listenerfmt(MFSC *op,char *rbuf,int rlen,int idx,
		MFSMSG_LISTENER *lp) noex {
	uint		rc = lp->rc ;
	int		rs = SR_OK ;
	int		wlen = 0 ;
	cchar		*fmt ;
	if (op == NULL) return SR_FAULT ;
	if (lp->name[0] != '\0') {
	    cint	ls = lp->ls ;
	    cchar	*sn ;
	    if (ls & LISTENSPEC_MDELPEND) {
	        sn = "D" ;
	    } else if (ls & LISTENSPEC_MBROKEN) {
	        sn = "B" ;
	    } else if (ls & LISTENSPEC_MACTIVE) {
	        sn = "A" ;
	    } else if (ls == 0) {
	        sn = "C" ;
	    } else {
	        sn = "U" ;
	    }
	    fmt = "i=%u %s type=%s addr=%s (%d)\n" ;
	    rs = bufprintf(rbuf,rlen,fmt,idx,sn,lp->name,lp->addr,ls) ;
	    wlen += rs ;
	} else {
	    fmt = "i=%u no-name rc=%d\n" ;
	    rs = bufprintf(rbuf,rlen,fmt,idx,rc) ;
	    wlen += rs ;
	} /* end if (name) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mfsc_listenerfmt) */

#ifdef	COMMENT

static int mfsc_spawn(MFSC *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		cs ;
	int		to_run = TO_RUN ;
	cchar		*argz = MFSC_FACNAME ;
	char		pbuf[MAXPATHLEN + 1] ;

	for (int i = 0 ; (rs >= 0) && (prbins[i] != NULL) ; i += 1) {
	    if ((rs = mkpath3(pbuf,op->pr,prbins[i],argz)) >= 0) {
	        rs = perm(pbuf,-1,-1,NULL,X_OK) ;
	    }
	} /* end for */

	if (rs >= 0) {
	    ENVMGR	em ;
	    if ((rs = envmgr_start(&em)) >= 0) {
	        if ((rs = mfsc_envload(op,&em)) >= 0) {
	            cint	dlen = DIGBUFLEN ;
	            char	dbuf[DIGBUFLEN + 1] ;
	            if ((rs = ctdeci(dbuf,dlen,to_run)) >= 0) {
	                char	optbuf[OPTBUFLEN + 1] ;
	                if ((rs = sncpy2(optbuf,OPTBUFLEN,"-d=",dbuf)) >= 0) {
	                    int		i = 0 ;
	                    cchar	*av[6] ;
	                    cchar	**ev ;
	                    av[i++] = argz ;
	                    av[i++] = optbuf ;
	                    av[i++] = "-o" ;
	                    av[i++] = "quick" ;
	                    av[i++] = NULL ;
	                    if ((rs = envmgr_getvec(&em,&ev)) >= 0) {
	                        SPAWNPROC	ps ;
	                        memset(&ps,0,sizeof(SPAWNPROC)) ;
	                        ps.opts = 0 ;
	                        ps.opts |= SPAWNPROC_OSETSID ;
	                        ps.opts |= SPAWNPROC_OSIGDEFS ;
	                        ps.disp[0] = SPAWNPROC_DCLOSE ;
	                        ps.disp[1] = SPAWNPROC_DCLOSE ;
	                        ps.disp[2] = SPAWNPROC_DCLOSE ;
	                        if ((rs = spawnproc(&ps,pbuf,av,ev)) >= 0) {
	                            const pid_t	pid = rs ;
	                            u_waitpid(pid,&cs,0) ;
	                        } /* end if */
	                    } /* end if (envmgr_getvec) */
	                } /* end if (sncpy) */
	            } /* end if (ctdeci) */
	        } /* end if (mfsc_envload) */
	        rs1 = envmgr_finish(&em) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (envmgr) */
	} /* end if (ok) */

	return rs ;
}
/* end subroutine (mfsc_spawn) */

static int mfsc_envload(MFSC *op,ENVMGR *emp) noex {
	int		rs ;
	if ((rs = envmgr_set(emp,VARMFSQUIET,"1",1)) >= 0) {
	    rs = envmgr_set(emp,VARMFSPR,op->pr,-1) ;
	}
	return rs ;
}
/* end subroutine (mfsc_envload) */

#endif /* COMMENT */

static int mksrvdname(char *rbuf,cchar *td,cchar *pr,cchar *fn) noex {
	cint	rlen = MAXPATHLEN ;
	int		rs = SR_OK ;
	int		i = 0 ;

	if (rs >= 0) {
	    rs = storebuf_strw(rbuf,rlen,i,td,-1) ;
	    i += rs ;
	}

	if (rs >= 0) {
	    rs = storebuf_chr(rbuf,rlen,i,'/') ;
	    i += rs ;
	}

	{
	    int		cl ;
	    cchar	*cp ;
	    if ((cl = sfbasename(pr,-1,&cp)) > 0) {
	        rs = storebuf_strw(rbuf,rlen,i,cp,cl) ;
	        i += rs ;
	    }
	}

	if (rs >= 0) {
	    rs = storebuf_chr(rbuf,rlen,i,'/') ;
	    i += rs ;
	}

	if (rs >= 0) {
	    rs = storebuf_strw(rbuf,rlen,i,fn,-1) ;
	    i += rs ;
	}

	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mksrvdname) */



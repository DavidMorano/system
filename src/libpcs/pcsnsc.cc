/* pcsnsc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* PCS Name-Server-Client */
/* object to interact with the PCS server */

#define	CF_DEBUG	0		/* compile-time debugging */

/* revision history:

	= 2000-12-18, David A­D­ Morano
	This object module was first written.

	= 2011-01-25, David A­D­ Morano
	I added the capability to also send the 'mark', 'report',
	and 'exit' commands to the server.  Previously these were
	not implemented here.

*/

/* Copyright © 2000,2011 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	pcsnsc

	Description:
	This module mediates (as a sort of client) the interactions
	with the PCS server.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<estrings.h>		/* LIBUC */
#include	<vechand.h>		/* LIBUC */
#include	<storebuf.h>		/* LIBUC */
#include	<sockaddress.h>		/* LIBUC */
#include	<envmgr.h>		/* LIBUC */
#include	<spawnproc.h>		/* LIBUC */
#include	<ctdec.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"pcsnsc.h"
#include	"pcsmsg.h"
#include	"pcsnsreq.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	PCSNSC_VARPR	"PCS"
#define	PCSNSC_FACNAME	"pcs"
#define	PCSNSC_PIDNAME	"pid"
#define	PCSNSC_REQNAME	"req"
#define	PCSNSC_DMODE	0777

#define	VARPREXTRA	"EXTRA" ;
#define	VARPCSQUIET	"PCS_QUIET"
#define	VARPCSPR	"PCS_PROGRAMROOT"

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

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* compile-time debugging */
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	pcsnsc_setbegin(PCSNSC *,cchar *) noex ;
local int	pcsnsc_setend(PCSNSC *) noex ;
local int	pcsnsc_srvdname(PCSNSC *,char *) noex ;
local int	pcsnsc_srvfname(PCSNSC *,cchar *) noex ;
local int	pcsnsc_bind(PCSNSC *,int,cchar *) noex ;
local int	pcsnsc_bufbegin(PCSNSC *) noex ;
local int	pcsnsc_bufend(PCSNSC *) noex ;
local int	pcsnsc_connect(PCSNSC *) noex ;
local int	pcsnsc_istatus(PCSNSC *,PCSNSC_STATUS *) noex ;

#ifdef	COMMENT
local int	pcsnsc_spawn(PCSNSC *) noex ;
local int	pcsnsc_envload(PCSNSC *,ENVMGR *) noex ;
#endif

local int	mksrvdname(char *,cchar *,cchar *,cchar *) noex ;


/* local variables */

#ifdef	COMMENT
constexpr cpcchar	prbins[] = {
	"bin",
	"sbin",
	nullptr
} ; /* end array */
#endif /* COMMENT */


/* exported variables */

constexpr PCSNSC_OBJ	pcsnsc_modinfo = {
	"pcsnsc",
	szof(PCSNSC),
	0
} ; /* end if (object) */


/* exported variables */


/* exported subroutines */

int pcsnsc_open(PCSNSC *op,cchar *pr,int to) noex {
	int		rs ;

	if (op == nullptr) return SR_FAULT ;
	if (pr == nullptr) return SR_FAULT ;
	if (pr[0] == '\0') return SR_INVALID ;

	if (to < 1) to = 1 ;
	DEBUGPRINTF("ent pr=%s to=%d\n",pr,to) ;
	memclear(op) ;
	op->to = to ;
	op->fd = -1 ;
	op->pid = getpid() ;

	if ((rs = pcsnsc_setbegin(op,pr)) > 0) {
	    if ((rs = pcsnsc_connect(op)) > 0) {
	        if ((rs = pcsnsc_bufbegin(op)) >= 0) {
	            op->fl.srv = true ;
		    rs = 1 ;
	            op->magval = PCSNSC_MAGIC ;
	        }
	    }
	    if ((rs < 0) || (! op->fl.srv)) {
	        pcsnsc_setend(op) ;
	    } /* end if (error) */
	} /* end if (pcsnsc-set) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (pcsnsc_open) */

int pcsnsc_close(PCSNSC *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != PCSNSC_MAGIC) return SR_NOTOPEN ;

	rs1 = pcsnsc_bufend(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = pcsnsc_setend(op) ;
	if (rs >= 0) rs = rs1 ;

	DEBUGPRINTF("ret rs=%d\n",rs) ;

	op->magval = 0 ;
	return rs ;
} /* end subroutine (pcsnsc_close) */

int pcsnsc_status(PCSNSC *op,PCSNSC_STATUS *statp) noex {
	int		rs ;

	DEBUGPRINTF("ent\n") ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != PCSNSC_MAGIC) return SR_NOTOPEN ;

	DEBUGPRINTF("con\n") ;

	rs = pcsnsc_istatus(op,statp) ;

	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (pcsnsc_status) */

int pcsnsc_help(PCSNSC *op,char *rbuf,int rlen,int idx) noex {
	int		rs = SR_OK ;
	int		rl = 0 ;

	DEBUGPRINTF("ent\n") ;

	if (op == nullptr) return SR_FAULT ;
	if (rbuf == nullptr) return SR_FAULT ;

	if (op->magval != PCSNSC_MAGIC) return SR_NOTOPEN ;

	if (op->fl.srv) {
	    struct pcsmsg_gethelp	mreq ;
	    struct pcsmsg_help		mres ;
	    cint		to = op->to ;
	    cint		mlen = op->mlen ;
	    char		*mbuf = op->mbuf ;
	    mreq.tag = op->pid ;
	    mreq.idx = (uchar) idx ;
	    if ((rs = pcsmsg_gethelp(&mreq,0,mbuf,mlen)) >= 0) {
	        cint	fd = op->fd ;
	        if ((rs = u_send(fd,mbuf,rs,0)) >= 0) {
	            cint	mf = 0 ;
	            cint	ro = FM_TIMED ;
	            if ((rs = uc_recve(fd,mbuf,mlen,mf,to,ro)) >= 0) {
	                if ((rs = pcsmsg_help(&mres,1,mbuf,rs)) >= 0) {
	                    if (mres.rc == pcsmsgrc_ok) {
	                        cchar	*rp = mres.val ;
	                        rl = mres.vl ;
	                        rs = snwcpy(rbuf,rlen,rp,rl) ;
	                    } else if (mres.rc == pcsmsgrc_notfound) {
	                        rl = 0 ;
	                    } else {
	                        rs = SR_BADMSG ;
	                    }
	                }
		    } else if (isBadRecv(rs)) {
		        op->fl.srv = false ;
		        rs = SR_OK ;
	            } /* end if (uc_recve) */
		} else if (isBadSend(rs)) {
		    op->fl.srv = false ;
		    rs = SR_OK ;
	        } /* end if (u_send) */
	    } /* end if (pcsmsg_gethelp) */
	    if (rs < 0) op->fl.srv = false ;
	} /* end if (servicing) */

	DEBUGPRINTF("ret rs=%d rl=%u\n",rs,rl) ;
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (pcsnsc_help) */

int pcsnsc_getval(PCSNSC *op,char *rbuf,int rlen,cchar *un,int w) noex {
	int		rs = SR_OK ;
	int		rl = 0 ;
	DEBUGPRINTF("ent un=%s w=%u\n",un,w) ;
	if (op == nullptr) return SR_FAULT ;
	if (rbuf == nullptr) return SR_FAULT ;
	if (un == nullptr) return SR_FAULT ;

	if (un[0] == '\0') return SR_INVALID ;

	if (op->magval != PCSNSC_MAGIC) return SR_NOTOPEN ;

	if (op->fl.srv) {
	    struct pcsmsg_getval	mreq ;
	    struct pcsmsg_val		mres ;
	    cint		to = op->to ;
	    cint		mlen = op->mlen ;
	    char		*mbuf = op->mbuf ;
	    mreq.tag = op->pid ;
	    mreq.w = (uchar) w ;
	    strwcpy(mreq.key,un,PCSMSG_KEYLEN) ;
	    if ((rs = pcsmsg_getval(&mreq,0,mbuf,mlen)) >= 0) {
	        cint	fd = op->fd ;
	DEBUGPRINTF("fd=%d\n",fd) ;
	        if ((rs = u_send(fd,mbuf,rs,0)) >= 0) {
	            cint	mf = 0 ;
	            cint	ro = FM_TIMED ;
	            if ((rs = uc_recve(fd,mbuf,mlen,mf,to,ro)) >= 0) {
	                if ((rs = pcsmsg_val(&mres,1,mbuf,rs)) >= 0) {
	                    if (mres.rc == pcsmsgrc_ok) {
	                        cchar	*rp = mres.val ;
	                        rl = mres.vl ;
	                        rs = snwcpy(rbuf,rlen,rp,rl) ;
	                    } else if (mres.rc == pcsmsgrc_notfound) {
	                        rl = 0 ;
	                    } else {
	                        rs = SR_BADMSG ;
	                    }
	                }
		    } else if (isBadRecv(rs)) {
	DEBUGPRINTF("uc_recve() out rs=%d\n",rs) ;
		        op->fl.srv = false ;
		        rs = SR_OK ;
	            } /* end if (uc_recve) */
		} else if (isBadSend(rs)) {
	DEBUGPRINTF("u_send() out rs=%d\n",rs) ;
		    op->fl.srv = false ;
		    rs = SR_OK ;
	        } /* end if (u_send) */
	    } /* end if (pcsmsg_getval) */
	DEBUGPRINTF("pcsmsg_getval() out rs=%d\n",rs) ;
	    if (rs < 0) op->fl.srv = false ;
	} /* end if (servicing) */

	DEBUGPRINTF("ret rs=%d rl=%u\n",rs,rl) ;
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (pcsnsc_getval) */

int pcsnsc_mark(PCSNSC *op) noex {
	int		rs = SR_OK ;
	int		rl = 0 ;
	DEBUGPRINTF("ent\n") ;
	if (op == nullptr) return SR_FAULT ;

	if (op->magval != PCSNSC_MAGIC) return SR_NOTOPEN ;

	if (op->fl.srv) {
	    struct pcsmsg_mark	mreq ;
	    struct pcsmsg_ack	mres ;
	    cint		to = op->to ;
	    cint		mlen = op->mlen ;
	    char		*mbuf = op->mbuf ;
	    mreq.tag = op->pid ;
	    if ((rs = pcsmsg_mark(&mreq,0,mbuf,mlen)) >= 0) {
	        cint	fd = op->fd ;
	        if ((rs = u_send(fd,mbuf,rs,0)) >= 0) {
	            cint	mf = 0 ;
	            cint	ro = FM_TIMED ;
	            if ((rs = uc_recve(fd,mbuf,mlen,mf,to,ro)) >= 0) {
	                if ((rs = pcsmsg_ack(&mres,1,mbuf,rs)) >= 0) {
	                    if (mres.rc == pcsmsgrc_ok) {
	                        rl = 1 ;
	                    } else {
	                        rs = SR_BADMSG ;
	                    }
	                }
		    } else if (isBadRecv(rs)) {
		        op->fl.srv = false ;
		        rs = SR_OK ;
	            } /* end if (uc_recve) */
		} else if (isBadSend(rs)) {
		    op->fl.srv = false ;
		    rs = SR_OK ;
	        } /* end if (u_send) */
	    } /* end if (pcsmsg_mark) */
	    if (rs < 0) {
		op->fl.srv = false ;
	    } /* end if (error) */
	} /* end if (servicing) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (pcsnsc_mark) */

int pcsnsc_exit(PCSNSC *op,cchar *reason) noex {
	int		rs = SR_OK ;
	int		rl = 0 ;
	DEBUGPRINTF("ent\n") ;
	if (op == nullptr) return SR_FAULT ;

	if (op->magval != PCSNSC_MAGIC) return SR_NOTOPEN ;

	if (op->fl.srv) {
	    struct pcsmsg_exit	mreq ;
	    struct pcsmsg_ack	mres ;
	    cint		to = op->to ;
	    cint		mlen = op->mlen ;
	    char		*mbuf = op->mbuf ;
	    mreq.tag = op->pid ;
	    mreq.reason[0] = '\0' ;
	    if (reason != nullptr) {
	        strwcpy(mreq.reason,reason,REALNAMELEN) ;
	    }
	    if ((rs = pcsmsg_exit(&mreq,0,mbuf,mlen)) >= 0) {
	        cint	fd = op->fd ;
	        if ((rs = u_send(fd,mbuf,rs,0)) >= 0) {
	            cint	mf = 0 ;
	            cint	ro = FM_TIMED ;
	            if ((rs = uc_recve(fd,mbuf,mlen,mf,to,ro)) >= 0) {
	                if ((rs = pcsmsg_ack(&mres,1,mbuf,rs)) >= 0) {
	                    if (mres.rc == pcsmsgrc_ok) {
	                        rl = 1 ;
	                    } else {
	                        rs = SR_BADMSG ;
	                    }
	                }
		    } else if (isBadRecv(rs)) {
		        op->fl.srv = false ;
		        rs = SR_OK ;
	            } /* end if (uc_recve) */
		} else if (isBadSend(rs)) {
		    op->fl.srv = false ;
		    rs = SR_OK ;
	        } /* end if (u_send) */
	    } /* end if (pcsmsg_exit) */
	    if (rs < 0) {
		op->fl.srv = false ;
	    } /* end if (error) */
	} /* end if (servicing) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (pcsnsc_exit) */

int pcsnsc_getname(PCSNSC *op,char *rbuf,int rlen,cchar *un) noex {
	cint	w = pcsnsreq_pcsname ;
	return pcsnsc_getval(op,rbuf,rlen,un,w) ;
} /* end subroutine (pcsnsc_getname) */


/* local subroutines */

local int pcsnsc_setbegin(PCSNSC *op,cchar *pr) noex {
	int		rs ;
	int		f = false ;
	cchar		*cp ;
	DEBUGPRINTF("ent\n") ;
	if ((rs = uc_mallocstrw(pr,-1,&cp)) >= 0) {
	    char	rbuf[MAXPATHLEN+1] ;
	    op->pr = cp ;
	    if ((rs = pcsnsc_srvdname(op,rbuf)) > 0) {
	        if ((rs = pcsnsc_srvfname(op,rbuf)) > 0) {
	            if ((rs = pcsnsc_bind(op,true,rbuf)) >= 0) {
	                f = true ;
	            }
	            if ((rs < 0) && op->srvfname) {
	                uc_free(op->srvfname) ;
	                op->srvfname = nullptr ;
	            } /* end if (error) */
	        } /* end if (pcsnsc_srvfname) */
	    } /* end if (pcsnsc_srvdname) */
	    if (((rs < 0) || (!f)) && (op->pr != nullptr)) {
	        uc_free(op->pr) ;
	        op->pr = nullptr ;
	    } /* end if (error) */
	} /* end if (m-a) */

	return (rs >= 0) ? f : rs ;
} /* end subroutine (pcsnsc_setbegin) */

local int pcsnsc_setend(PCSNSC *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	rs1 = pcsnsc_bufend(op) ;
	if (rs >= 0) rs = rs1 ;
	}
	{
	rs1 = pcsnsc_bind(op,false,nullptr) ;
	if (rs >= 0) rs = rs1 ;
	}
	{
	if (op->srvfname != nullptr) {
	    rs1 = uc_free(op->srvfname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->srvfname = nullptr ;
	}
	if (op->pr != nullptr) {
	    rs1 = uc_free(op->pr) ;
	    if (rs >= 0) rs = rs1 ;
	    op->pr = nullptr ;
	}
	return rs ;
} /* end subroutine (pcsnsc_setend) */

local int pcsnsc_srvdname(PCSNSC *op,char *rbuf) noex {
	int		rs ;
	int		rl = 0 ;
	cchar		*td = TMPDNAME ;
	cchar		*fn = PCSNSC_FACNAME ;
	DEBUGPRINTF("ent\n") ;
	if ((rs = mksrvdname(rbuf,td,op->pr,fn)) >= 0) {
	    USTAT	sb ;
	    if ((rs = u_stat(rbuf,&sb)) >= 0) {
	        if (S_ISDIR(sb.st_mode)) {
	            cint	am = (W_OK|W_OK|X_OK) ;
	            if ((rs = perm(rbuf,-1,-1,nullptr,am)) >= 0) {
	                rl = 1 ;
	            } else if (isNotAccess(rs)) {
	                rs = SR_OK ;
	            }
	        }
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    }
	}
	DEBUGPRINTF("ret rs=%d rl=%u\n",rs,rl) ;
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (pcsnsc_srvdname) */

local int pcsnsc_srvfname(PCSNSC *op,cchar *srvdname) noex {
	int		rs ;
	int		rl = 0 ;
	cchar		*reqname = PCSNSC_REQNAME ;
	char		srvfname[MAXPATHLEN + 1] ;
	DEBUGPRINTF("ent\n") ;
	if ((rs = mkpath2(srvfname,srvdname,reqname)) >= 0) {
	    USTAT	sb ;
	    rl = rs ;
	DEBUGPRINTF("srvfname=%s\n",srvfname) ;
	    if ((rs = u_stat(srvfname,&sb)) >= 0) {
	        if (S_ISSOCK(sb.st_mode)) {
	            cint	am = (R_OK|W_OK) ;
	            if ((rs = perm(srvfname,-1,-1,nullptr,am)) >= 0) {
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
	DEBUGPRINTF("not-present rs=%d\n",rs) ;
	        rl = 0 ;
	        rs = SR_OK ;
	    }
	} /* end if (srvfname) */
	DEBUGPRINTF("ret rs=%d rl=%u\n",rs,rl) ;
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (pcsnsc_srvfname) */

local int pcsnsc_bind(PCSNSC *op,int f,cchar *srvdname) noex {
	int		rs = SR_OK ;
	int		f_err = false ;
	DEBUGPRINTF("ent f=%u\n",f) ;
	if (f) {
	    cchar	*tn = "clientXXXXXXXX" ;
	    char	template[MAXPATHLEN + 1] ;

	    if ((rs = mkpath2(template,srvdname,tn)) >= 0) {
	        const mode_t	om = (S_IFSOCK | 0666) ;
	        cint	of = (O_RDWR | O_CLOEXEC | O_MINMODE) ;
	        char		fname[MAXPATHLEN + 1] ;
	        if ((rs = opentmpusd(template,of,om,fname)) >= 0) {
	            cchar	*cp ;
			DEBUGPRINTF("opentmpusd() rs=%d\n",rs) ;
	            op->fd = rs ;
	            u_chmod(fname,om) ;
	            uc_closeonexec(op->fd,true) ;
	            if ((rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
	                op->srcfname = cp ;
	            } else {
	                f_err = true ;
	            }
	        } /* end if (opentmpusd) */
	    } /* end if (mkpath) */

	} /* end if (bind-on) */

	if ((! f) || f_err) {

	    if (op->fd >= 0) {
	        u_close(op->fd) ;
	        op->fd = -1 ;
	    }
	    if (op->srcfname != nullptr) {
	        if (op->srcfname[0] != '\0') {
	            u_unlink(op->srcfname) ;
	        }
	        uc_free(op->srcfname) ;
	        op->srcfname = nullptr ;
	    }

	} /* end if (bind-off) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (pcsnsc_bind) */

local int pcsnsc_bufbegin(PCSNSC *op) noex {
	cint	blen = MSGBUFLEN ;
	int		rs ;
	char		*bp ;
	if ((rs = uc_malloc((blen+1),&bp)) >= 0) {
	    op->mbuf = bp ;
	    op->mlen = blen ;
	}
	return rs ;
} /* end subroutine (pcsnsc_bufbegin) */

local int pcsnsc_bufend(PCSNSC *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->mbuf != nullptr) {
	    rs1 = uc_free(op->mbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    op->mbuf = nullptr ;
	    op->mlen = 0 ;
	}
	return rs ;
} /* end subroutine (pcsnsc_bufend) */

local int pcsnsc_connect(PCSNSC *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = false ;
	DEBUGPRINTF("ent srvfname=%s\n",op->srvfname) ;
	if (op->srvfname != nullptr) {
	    SOCKADDRESS	sa ;
	    cint	af = AF_UNIX ;
	    cchar	*sfn = op->srvfname ;
	    if ((rs = sockaddress_start(&sa,af,sfn,0,0)) >= 0) {
	        SOCKADDR	*sap = (SOCKADDR *) &sa ;
	        cint	sal = rs ;
	        cint	to = op->to ;
	        if ((rs = uc_connecte(op->fd,sap,sal,to)) >= 0) {
	            f = true ;
	        } else if (isFailConn(rs)) {
	            rs = SR_OK ;
	        }
	        rs1 = sockaddress_finish(&sa) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sockaddress) */
	} /* end if (non-null) */
	DEBUGPRINTF("ret rs=%d f=%u\n",rs,f) ;
	return (rs >= 0) ? f : rs ;
} /* end subroutine (pcsnsc_connect) */

local int pcsnsc_istatus(PCSNSC *op,PCSNSC_STATUS *statp) noex {
	int		rs = SR_OK ;
	int		rc = 0 ;
	DEBUGPRINTF("ent\n") ;
	if (statp) {
	    memclear(statp) ;
	}

	if (op->fl.srv) {
	    struct pcsmsg_getstatus	mreq ;
	    struct pcsmsg_status	mres ;
	    cint		to = op->to ;
	    cint		mlen = op->mlen ;
	    char		*mbuf = op->mbuf ;
	    mreq.tag = op->pid ;
	    if ((rs = pcsmsg_getstatus(&mreq,0,mbuf,mlen)) >= 0) {
	        cint	fd = op->fd ;
	        if ((rs = u_send(fd,mbuf,rs,0)) >= 0) {
	            cint	mf = 0 ;
	            cint	ro = FM_TIMED ;
	            if ((rs = uc_recve(fd,mbuf,mlen,mf,to,ro)) >= 0) {
	                if ((rs = pcsmsg_status(&mres,1,mbuf,rs)) >= 0) {
	                    DEBUGPRINTF("pcsmsg_status() rs=%d rc=%u\n",
				    rs,mres.rc) ;
	                    if (mres.rc == pcsmsgrc_ok) {
				rc = 1 ;
				if (statp != nullptr) {
	                            statp->pid = mres.pid ;
				    statp->queries = mres.queries ;
				}
	                    } else if (mres.rc == pcsmsgrc_notavail) {
	                        rc = 0 ;
	                    } else {
	                        rs = SR_BADMSG ;
	                    }
	                }
		    } else if (isBadRecv(rs)) {
		        op->fl.srv = false ;
		        rs = SR_OK ;
	            } /* end if (uc_recve) */
		DEBUGPRINTF("recv-out rs=%d\n",rs) ;
		} else if (isBadSend(rs)) {
		    op->fl.srv = false ;
		    rs = SR_OK ;
	        } /* end if (u_send) */
		DEBUGPRINTF("send-out rs=%d\n",rs) ;
	    } /* end if (pcsmsg_getstatus) */
	    if (rs < 0) op->fl.srv = false ;
	} /* end if (servicing) */
	DEBUGPRINTF("ret rs=%d pid=%d\n",rs,rc) ;
	return (rs >= 0) ? rc : rs ;
} /* end subroutine (pcsnsc_istatus) */

#ifdef	COMMENT
local int pcsnsc_spawn(PCSNSC *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		cs ;
	int		i ;
	int		to_run = TO_RUN ;
	cchar	*argz = PCSNSC_FACNAME ;
	char		pbuf[MAXPATHLEN + 1] ;

	for (i = 0 ; (rs >= 0) && (prbins[i] != nullptr) ; i += 1) {
	    if ((rs = mkpath3(pbuf,op->pr,prbins[i],argz)) >= 0) {
	        rs = perm(pbuf,-1,-1,nullptr,X_OK) ;
	    }
	} /* end for */

	if (rs >= 0) ylikely {
	    ENVMGR	em ;
	    if ((rs = envmgr_start(&em)) >= 0) ylikely {
	        if ((rs = pcsnsc_envload(op,&em)) >= 0) ylikely {
		    cint	dlen = DIGBUFLEN ;
	            char	dbuf[DIGBUFLEN + 1] ;
	            if ((rs = ctdeci(dbuf,dlen,to_run)) >= 0) ylikely {
	                char	optbuf[OPTBUFLEN + 1] ;
	                if ((rs = sncpy2(optbuf,OPTBUFLEN,"-d=",dbuf)) >= 0) {
	                    int		i = 0 ;
	                    cchar	*av[6] ;
	                    cchar	**ev ;
	                    av[i++] = argz ;
	                    av[i++] = optbuf ;
	                    av[i++] = "-o" ;
	                    av[i++] = "quick" ;
	                    av[i++] = nullptr ;
	                    if ((rs = envmgr_getvec(&em,&ev)) >= 0) {
	                        SPAWNPROC	ps = {} ;
				ps.opts = SPAWNPROC_OSETSID ;
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
	        } /* end if (pcsnsc_envload) */
	        rs1 = envmgr_finish(&em) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (envmgr) */
	} /* end if (ok) */

	return rs ;
} /* end subroutine (pcsnsc_spawn) */

local int pcsnsc_envload(PCSNSC *op,ENVMGR *emp) noex {
	int		rs ;
	if ((rs = envmgr_set(emp,VARPCSQUIET,"1",1)) >= 0) {
	    rs = envmgr_set(emp,VARPCSPR,op->pr,-1) ;
	}
	return rs ;
} /* end subroutine (pcsnsc_envload) */

#endif /* COMMENT */

local int mksrvdname(char *rbuf,cchar *td,cchar *pr,cchar *fn) noex {
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
} /* end subroutine (mksrvdname) */



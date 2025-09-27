/* pcsadj SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* PCS adjunct subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-05-14, David AÂ­DÂ­ Morano
	Originally written for Rightcore Network Services.

	= 2005-01-25, David AÂ­DÂ­ Morano
	This code was separated out from the main code (in 'pcsmain.c')
	due to conflicts over including different versions of the
	system socket structures.

*/

/* Copyright © 2004,2005 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	pcsadj

	Description:
	This is adjunct code to the main PCS program.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/socket.h>
#include	<sys/uio.h>
#include	<sys/msg.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<netdb.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<msfile.h>
#include	<sockaddress.h>
#include	<conmsghdr.h>
#include	<pcsns.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"shio.h"
#include	"msgdata.h"
#include	"pcsmain.h"
#include	"pcslocinfo.h"
#include	"pcslog.h"
#include	"pcscmd.h"
#include	"defs.h"
#include	"pcsmsg.h"


/* local defines */

#ifndef	PROGINFO
#define	PROGINFO	PROGINFO
#endif

#define	PCSADJ		struct pcsadj
#define	PCSADJ_FL	struct pcsadj_fl

#ifndef	POLL_INTMULT
#define	POLL_INTMULT	1000
#endif

#ifndef	PBUFLEN
#define	PBUFLEN		(4 * MAXPATHLEN)
#endif

#ifndef	VBUFLEN
#define	VBUFLEN		(4 * MAXPATHLEN)
#endif

#ifndef	EBUFLEN
#define	EBUFLEN		(3 * MAXPATHLEN)
#endif


/* external subroutines */


/* external variables */


/* local structures */

struct pcsadj_fl {
	uint		m:1 ;
} ;

struct pcsadj {
	PCSADJ_FL	open ;
	MSGDATA		m ;
} ;


/* forward references */

static int	pcsadj_allocbegin(PROGINFO *) noex ;
static int	pcsadj_allocend(PROGINFO *) noex ;

static int	pcsadj_objbegin(PROGINFO *) noex ;
static int	pcsadj_objend(PROGINFO *) noex ;

static int	pcsadj_reqother(PROGINFO *,int) noex ;
static int	pcsadj_reqmsg(PROGINFO *,int) noex ;

static int	pcsadj_getstatus(PROGINFO *,MSGDATA *) noex ;
static int	pcsadj_gethelp(PROGINFO *,MSGDATA *) noex ;
static int	pcsadj_getval(PROGINFO *,MSGDATA *) noex ;
static int	pcsadj_mark(PROGINFO *,MSGDATA *) noex ;
static int	pcsadj_exit(PROGINFO *,MSGDATA *) noex ;

static int	pcsadj_send(PROGINFO *,MSGDATA *,int,uint) noex ;
static int	pcsadj_invalid(PROGINFO *,MSGDATA *,int,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int pcsadj_begin(PROGINFO *pip) noex {
	LOCINFO		*lip = pip->lip ;
	int		rs = SR_OK ;

	lip->rfd = -1 ;
	if (lip->fl.adj) {

	    if (lip->reqfname == NULL) {
	        rs = locinfo_reqfname(lip) ;
	    }

	    if (pip->debuglevel > 0) {
	        shio_printf(pip->efp,"%s: req=%s\n",
	            pip->progname,lip->reqfname) ;
	    }

	    if (pip->open.logprog) {
	        logprintf(pip,"req=%s",lip->reqfname) ;
	    }

	    if (rs >= 0) {
	        cmode	om = 0666 ;
	        int	opts = 0 ;
	        if (lip->fl.reuseaddr) opts |= 1 ; /* reuse-address */
	        if ((rs = listenusd(lip->reqfname,om,opts)) >= 0) {
	            cint	fd = rs ;
	            if ((rs = uc_closeonexec(fd,TRUE)) >= 0) {
	                if ((rs = pcsadj_allocbegin(pip)) >= 0) {
	                    lip->rfd = fd ;
	                    lip->open.adj = TRUE ;
	                }
	            }
	            if (rs < 0)
	                u_close(fd) ;
	        } /* end if (listenusd) */
	    } /* end if (ok) */

	} /* end if (adj) */

	return rs ;
}
/* end subroutine (pcsadj_begin) */

int pcsadj_end(PROGINFO *pip) noex {
	LOCINFO		*lip = pip->lip ;
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = pcsadj_allocend(pip) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (lip->rfd >= 0) {
	    rs1 = u_close(lip->rfd) ;
	    if (rs >= 0) rs = rs1 ;
	    lip->rfd = -1 ;
	}
	if (lip->reqfname) {
	    if (lip->reqfname[0] != '\0') {
	        u_unlink(lip->reqfname) ;
	    }
	}
	lip->open.adj = FALSE ;
	return rs ;
}
/* end subroutine (pcsadj_end) */

int pcsadj_req(PROGINFO *pip,int re) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if (re != 0) {
	    if ((re & POLLIN) || (re & POLLPRI)) {
	        c += 1 ;
	        rs = pcsadj_reqmsg(pip,re) ;
	    } else {
	        rs = pcsadj_reqother(pip,re) ;
	    }
	    if (rs > 0) {
	        logflush(pip) ;
	    }
	} /* end if (events) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (pcsadj_req) */


/* local subroutines */

static int pcsadj_allocbegin(PROGINFO *pip) noex {
	LOCINFO		*lip = pip->lip ;
	cint		asize = sizeof(PCSADJ) ;
	int		rs ;
	void		*p ;
	if ((rs = uc_malloc(asize,&p)) >= 0) {
	    lip->adj = p ;
	    rs = pcsadj_objbegin(pip) ;
	    if (rs < 0)
	        uc_free(p) ;
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (pcsadj_allocbegin) */

static int pcsadj_allocend(PROGINFO *pip) noex {
	LOCINFO		*lip = pip->lip ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (lip->adj != NULL) {
	    rs1 = pcsadj_objend(pip) ;
	    if (rs >= 0) rs = rs1 ;
	    rs1 = uc_free(lip->adj) ;
	    if (rs >= 0) rs = rs1 ;
	    lip->adj = NULL ;
	}
	return rs ;
}
/* end subroutine (pcsadj_allocend) */

static int pcsadj_objbegin(PROGINFO *pip) noex {
	LOCINFO		*lip = pip->lip ;
	int		rs ;
	if (lip->adj != NULL) {
	    PCSADJ	*pap = (PCSADJ *) lip->adj ;
	    if ((rs = msgdata_init(&pap->m,0)) >= 0) {
	        pap->open.m = TRUE ;
	    }
	}
	return rs ;
}
/* end subroutine (pcsadj_objbegin) */

static int pcsadj_objend(PROGINFO *pip) noex {
	PCSADJ		*pap ;
	LOCINFO		*lip = pip->lip ;
	int		rs = SR_OK ;
	int		rs1 ;

	if (lip->adj != NULL) {
	    pap = (PCSADJ *) lip->adj ;
	    if (pap->open.m) {
	        pap->open.m = FALSE ;
	        rs1 = msgdata_fini(&pap->m) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	}

	return rs ;
}
/* end subroutine (pcsadj_objend) */

static int pcsadj_reqother(PROGINFO *pip,int re) noex {
	int		rs = SR_OK ;
	int		f_logged = FALSE ;
	cchar		*ccp = NULL ;

	if (re & POLLHUP) {
	    ccp = "hangup" ;
	    rs = SR_HANGUP ;
	} else if (re & POLLERR) {
	    ccp = "error" ;
	    rs = SR_POLLERR ;
	} else if (re & POLLNVAL) {
	    ccp = "invalid" ;
	    rs = SR_NOTOPEN ;
	}

	if ((rs < 0) && (ccp != NULL) && pip->open.logprog) {
	    cchar	*fmt = "%s: IPC port condition=%s" ;
	    f_logged = TRUE ;
	    logprintf(pip,fmt,pip->progname,ccp) ;
	}

	return (rs >= 0) ? f_logged : rs ;
}
/* end subroutine (pcsadj_reqother) */

static int pcsadj_reqmsg(PROGINFO *pip,int re) noex {
	LOCINFO		*lip = pip->lip ;
	PCSADJ		*pap ;
	MSGDATA		*mdp ;
	int		rs ;
	int		f_logged = FALSE ;
	char		*mbuf ;
	(void) re ;
	pap = (PCSADJ *) lip->adj ;
	mdp = &pap->m ;
	if ((rs = msgdata_getbuf(mdp,&mbuf)) >= 0) {
	    if ((rs = msgdata_recv(mdp,lip->rfd)) > 0) {
	        if ((rs = msgdata_conpass(mdp,FALSE)) >= 0) {
		    if ((rs = locinfo_newreq(lip,1)) >= 0) {
			cint	nreqs = rs ;
	                int	mtype = MKCHAR(mbuf[0]) ;
	                lip->ti_lastreq = pip->daytime ;
	                switch (mtype) {
	                case pcsmsgtype_getstatus:
	                    rs = pcsadj_getstatus(pip,mdp) ;
	                    break ;
	                case pcsmsgtype_gethelp:
	                    rs = pcsadj_gethelp(pip,mdp) ;
	                    break ;
	                case pcsmsgtype_getval:
	                    rs = pcsadj_getval(pip,mdp) ;
	                    break ;
	                case pcsmsgtype_mark:
	                    f_logged = TRUE ;
	                    rs = pcsadj_mark(pip,mdp) ;
	                    break ;
	                case pcsmsgtype_exit:
	                    rs = pcsadj_exit(pip,mdp) ;
	                    break ;
	                default:
	                    f_logged = TRUE ;
	                    rs = pcsadj_invalid(pip,mdp,SR_INVALID,FALSE) ;
	                    break ;
	                } /* end switch */
	                if (mdp->ns >= 0) {
	                    u_close(mdp->ns) ;
	                    mdp->ns = -1 ;
	                }
		    } /* end if (locinfo_newreq) */
	        } /* end if (msgdata_conpass) */
	    } /* end if (msgdata_recv) */
	} /* end if (msgdata_recv) */

	return (rs >= 0) ? f_logged : rs ;
}
/* end subroutine (pcsadj_reqmsg) */

static int pcsadj_getstatus(PROGINFO *pip,MSGDATA *mdp) noex {
	struct pcsmsg_status	m0 ;
	struct pcsmsg_getstatus	m1 ;
	int		rs ;

	if ((rs = pcsmsg_getstatus(&m1,1,mdp->mbuf,mdp->ml)) >= 0) {
	    LOCINFO	*lip = pip->lip ;
	    if ((rs = locinfo_getreqs(lip)) >= 0) {
		cint	nreqs = rs ;
#ifdef	OPTIONAL
	        memset(&m0,0,sizeof(struct pcsmsg_status)) ;
#endif
	        m0.tag = m1.tag ;
	        m0.pid = pip->pid ;
	        m0.queries = nreqs ;
	        m0.rc = pcsmsgrc_ok ;
	        if ((rs = pcsmsg_status(&m0,0,mdp->mbuf,mdp->mlen)) >= 0) {
	            rs = pcsadj_send(pip,mdp,rs,m0.tag) ;
	        } /* end if */
	    } /* end if (locinfo_getreqs) */
	} else if (isBadMsg(rs)) {
	    rs = pcsadj_invalid(pip,mdp,rs,TRUE) ;
	}

	return rs ;
}
/* end subroutine (pcsadj_getstatus) */

static int pcsadj_gethelp(PROGINFO *pip,MSGDATA *mdp) noex {
	struct pcsmsg_gethelp	mreq ;
	struct pcsmsg_help	mres ;
	int		rs ;

	if ((rs = pcsmsg_gethelp(&mreq,1,mdp->mbuf,mdp->ml)) >= 0) {
	    cint	rsn = SR_NOTFOUND ;
	    cint	idx = mreq.idx ;
	    cchar	*np ;
	    memset(&mres,0,sizeof(struct pcsmsg_help)) ;
	    mres.tag = mreq.tag ;
	    if ((rs = pcscmd_svcname(pip,idx,&np)) >= 0) {
	        mres.rc = pcsmsgrc_ok ;
	        mres.vl = rs ;
	        strwcpy(mres.val,np,rs) ;
	    } else if (rs == rsn) {
	        mres.rc = pcsmsgrc_notfound ;
		rs = SR_OK ;
	    }
	    if (rs >= 0) {
	        cint	mlen = mdp->mlen ;
	        char	*mbuf = mdp->mbuf ;
	        if ((rs = pcsmsg_help(&mres,0,mbuf,mlen)) >= 0) {
	            rs = pcsadj_send(pip,mdp,rs,mreq.tag) ;
	        } /* end if */
	    } /* end if (ok) */
	} else if (isBadMsg(rs)) {
	    rs = pcsadj_invalid(pip,mdp,rs,TRUE) ;
	}

	return rs ;
}
/* end subroutine (pcsadj_gethelp) */

static int pcsadj_getval(PROGINFO *pip,MSGDATA *mdp) noex {
	struct pcsmsg_getval	mreq ;
	struct pcsmsg_val	mres ;
	LOCINFO		*lip = pip->lip ;
	int		rs ;

	if ((rs = pcsmsg_getval(&mreq,1,mdp->mbuf,mdp->ml)) >= 0) {
	    cint	rsn = SR_NOTFOUND ;
	    cint	w = MKCHAR(mreq.w) ;
	    cint	rlen = REALNAMELEN ;
	    int		vl = 0 ;
	    cchar	*key = mreq.key ;
	    char	*rbuf = mres.val ;
	    memset(&mres,0,sizeof(struct pcsmsg_val)) ;
	    mres.tag = mreq.tag ;
	    mres.w = (uchar) w ;
	    mreq.key[PCSMSG_KEYLEN] = '\0' ;
	    if ((rs = locinfo_nslook(lip,rbuf,rlen,key,w)) >= 0) {
	        vl = rs ;
	        mres.rc = pcsmsgrc_ok ;
	        mres.vl = (uchar) rs ;
	    } else if (rs == rsn) {
	        rs = SR_OK ;
	        mres.rc = pcsmsgrc_notfound ;
	    }
	    if (rs >= 0) {
	        cint	mlen = mdp->mlen ;
	        char	*mbuf = mdp->mbuf ;
	        if ((rs = pcsmsg_val(&mres,0,mbuf,mlen)) >= 0) {
	            char	tbuf[TIMEBUFLEN+1] ;
	            rs = pcsadj_send(pip,mdp,rs,mreq.tag) ;
	            timestr_logz(pip->daytime,tbuf) ;
	            if ((rs == 0) || (mres.rc != pcsmsgrc_ok)) vl = 0 ;
	            logprintf(pip,"%s req k=%s w=%u vl=%u",tbuf,key,w,vl) ;
	        } /* end if (pcsmsg_val) */
	    } /* end if (ok) */
	} else if (isBadMsg(rs)) {
	    rs = pcsadj_invalid(pip,mdp,rs,TRUE) ;
	}

	return rs ;
}
/* end subroutine (pcsadj_getval) */

static int pcsadj_mark(PROGINFO *pip,MSGDATA *mdp) noex {
	struct pcsmsg_mark	mreq ;
	struct pcsmsg_ack	mres ;
	int		rs ;

	if ((rs = pcsmsg_mark(&mreq,1,mdp->mbuf,mdp->ml)) >= 0) {
	    mres.tag = mreq.tag ;
	    mres.rc = 0 ;
	    if ((rs = pcsmsg_ack(&mres,0,mdp->mbuf,mdp->mlen)) >= 0) {
	        if ((rs = pcsadj_send(pip,mdp,rs,mreq.tag)) > 0) {
	            LOCINFO		*lip = pip->lip ;
	            const time_t	dt = pip->daytime ;
	            long 		lw = 0 ;
	            if (pip->intrun > (dt-lip->ti_start)) {
	                lw = (pip->intrun - (dt-lip->ti_start)) ;
	            }
	            logmark(pip,lw) ;
	        }
	    } /* end if */
	} else if (isBadMsg(rs)) {
	    rs = pcsadj_invalid(pip,mdp,rs,TRUE) ;
	}

	return rs ;
}
/* end subroutine (pcsadj_mark) */

static int pcsadj_exit(PROGINFO *pip,MSGDATA *mdp) noex {
	struct pcsmsg_exit	mreq ;
	struct pcsmsg_ack	mres ;
	int		rs ;

	if ((rs = pcsmsg_exit(&mreq,1,mdp->mbuf,mdp->ml)) >= 0) {
	    mres.tag = mreq.tag ;
	    mres.rc = 0 ;
	    if ((rs = pcsmsg_ack(&mres,0,mdp->mbuf,mdp->mlen)) >= 0) {
	        if ((rs = pcsadj_send(pip,mdp,rs,mreq.tag)) > 0) {
	            LOCINFO	*lip = pip->lip ;
	            rs = locinfo_reqexit(lip,mreq.reason) ;
	        }
	    } /* end if */
	} else if (isBadMsg(rs)) {
	    rs = pcsadj_invalid(pip,mdp,rs,TRUE) ;
	}

	return rs ;
}
/* end subroutine (pcsadj_exit) */

static int pcsadj_invalid(PROGINFO *pip,MSGDATA *mdp,int mrs,int f) noex {
	struct pcsmsg_status	mres ;
	int		rs ;
	cchar		*s = ((f) ? "invalid" : "bad-fmt") ;

	logprintf(pip,"%s client message (%d)",s,mrs) ;

	memset(&mres,0,sizeof(struct pcsmsg_status)) ;
	mres.pid = pip->pid ;
	mres.tag = 0 ;
	mres.rc = ((f) ? pcsmsgrc_invalid : pcsmsgrc_badfmt) ;

	if ((rs = pcsmsg_status(&mres,0,mdp->mbuf,mdp->mlen)) >= 0) {
	    rs = pcsadj_send(pip,mdp,rs,mres.tag) ;
	} /* end if */

	return rs ;
}
/* end subroutine (pcsadj_invalid) */

static int pcsadj_send(PROGINFO *pip,MSGDATA *mdp,int dl,uint tag) noex {
	LOCINFO		*lip = pip->lip ;
	int		rs ;
	int		len = 0 ;
	if ((rs = msghdr_send(mdp,lip->rfd,dl,0)) >= 0) {
	    len = rs ;
	} else if (isBadSend(rs)) {
	    logprintf(pip,"send failure t=%08x (%d)",tag,rs) ;
	    rs = SR_OK ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (pcsadj_send) */



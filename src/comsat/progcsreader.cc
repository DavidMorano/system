/* progcsreader SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to read the messages coming in */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable */
#define	CF_DEBUG	0		/* switchable debug print-outs */
#define	CF_DEBUGN	0		/* special debug print-outs */
#define	CF_DISP		1		/* dispatch */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	The program was written from scratch to do what the previous
	program by the same name did.  It used pieces from other
	(similar in some ways) programs.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	progcsreader

	Description:
	This subroutine reads in the COMAST messages and disposes
	of them appropriately.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstdarg>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<bfile.h>
#include	<vecstr.h>
#include	<vecobj.h>
#include	<ids.h>
#include	<psem.h>
#include	<hdrdecode.h>
#include	<char.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"
#include	"fsi.h"
#include	"upt.h"


/* local defines */

#ifndef	LINEBUFLEN
#define	LINEBUFLEN	MAX((2 * MAXPATHLEN),2048)
#endif

#ifndef	EBUFLEN
#define	EBUFLEN		100
#endif

#define	DISP		struct disp

#ifndef	POLL_INTMULT
#define	POLL_INTMULT	1000		/* poll-time multiplier */
#endif

#define	NFDS		2
#define	PG_POLLREG	(POLLIN | POLLPRI | POLLERR)

#define	NDF		"progcsreader.deb"

#ifndef	TO_READ
#define	TO_READ		2
#endif

#define	SUBINFO		struct subinfo


/* typedefs */

typedef int (*workthr)(void *) ;


/* external subroutines */

extern int	progcsmsg(PROGINFO *,cchar *,int) noex ;
extern int	progexit(PROGINFO *) noex ;
extern int	progloglock_begin(PROGINFO *) noex ;
extern int	progloglock_printf(PROGINFO *,cchar *,...) noex ;
extern int	progloglock_end(PROGINFO *) noex ;
extern int	progloglock_maint(PROGINFO *) noex ;

extern int	strlinelen(cchar *,int,int) noex ;

#if	CF_DEBUGS || CF_DEBUG
extern int	debugprintf(cchar *,...) noex ;
#endif


/* external variables */


/* local structures */

struct subinfo {
	PROGINFO	*pip ;
	ids		id ;
	HDRDECODE	d ;		/* header-decode */
	ptm		dm ;		/* header-decode mutex */
} ;

struct disp {
	PROGINFO	*pip ;
	SUBINFO		*sip ;
	FSI		wq ;
	PSEM		wq_sem ;
	vecobj		tids ;
	volatile int	f_exit ;
	volatile int	f_done ;
	int		n ;
} ;


/* forward references */

int 		progeprintf(PROGINFO *,cchar *,...) noex ;

local int	procreaders(PROGINFO *) noex ;

local int	subinfo_start(SUBINFO *,PROGINFO *) noex ;
local int	subinfo_finish(SUBINFO *) noex ;
local int	subinfo_hdr(SUBINFO *,wchar_t *,int,cchar *,int) noex ;
local int	subinfo_reader(SUBINFO *,DISP *) noex ;
local int	subinfo_procmsg(SUBINFO *,DISP *,cchar *,int) noex ;

#ifdef	COMMENT
local int	ereport(PROGINFO *,cchar *,int) noex ;
#endif

local int	disp_start(DISP *,PROGINFO *,SUBINFO *) noex ;
local int	disp_addwork(DISP *,cchar *,int) noex ;
local int	disp_finish(DISP *,int) noex ;
local int	disp_worker(DISP *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int progcsreader(PROGINFO *pip) noex {
	ptm		*emp = &pip->efm ;
	int		rs ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */

	if (isasocket(pip->fd_msg)) pip->fl.issocket = TRUE ;

#if	CF_DEBUGN
	nprintf(NDF,"progcsreader: f_issocket=%u fd_msg=%d\n",
	    pip->fl.issocket,pip->fd_msg) ;
#endif

#if	CF_DEBUG
	if (DEBUGLEVEL(3)) {
	    debugprintf("progcsreader: ent f_issocket=%u fd_msg=%d\n",
	        pip->fl.issocket,pip->fd_msg) ;
	    debugprintf("progcsreader: pn=%s\n",pip->progname) ;
	}
#endif /* CF_DEBUG */

	if ((rs = emp->create) >= 0) {
	    if ((rs = progloglock_begin(pip)) >= 0) {
	        if ((rs = termnote_open(&pip->tn,pip->pr)) >= 0) {
	            ptm *tmp = &pip->tmutex ;
	            if ((rs = tmp->create) >= 0) {
	                {
	                    rs = procreaders(pip) ;
			    rv = rs ;
	                }
	                tmp->destroy() ;
	            } /* end if (ptm) */
	            rs1 = termnote_close(&pip->tn) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (termnote) */
	        rs1 = progloglock_end(pip) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (progloglock) */
	    rs1 = emp->destroy ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("progcsreader: ret rs=%d\n",rs) ;
#endif

	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (progcsreader) */

int progeprintf(PROGINFO *pip,cchar *fmt,...) noex {
	va_list		ap ;
	int		rs = SR_OK ;
	if (pip->debuglevel > 0) {
	    ptm *emp = &pip->efm ;
	    if ((rs = emp->lockbegin) >= 0) {

#if	CF_DEBUG
	        if (DEBUGLEVEL(5))
	            debugprintf("progcsreader/progeprintf: fmt=>%s<\n",fmt) ;
#endif /* CF_DEBUG */

	        va_begin(ap,fmt) ;
	        rs = bvprintf(pip->efp,fmt,ap) ;
	        va_end(ap) ;

	        emp->lockend() ;
	    } /* end if (error-file lock) */
	} /* end if (debugging) */
	return rs ;
}
/* end subroutine (progeprintf) */


/* local subroutines */

local int procreaders(PROGINFO *pip) noex {
	SUBINFO		si, *sip = &si ;
	int		rs ;
	int		rs1 ;

	if ((rs = subinfo_start(sip,pip)) >= 0) {
	    DISP	disp ;
	    int		f ;
	    cchar	*cp ;

#if	CF_DISP
	    if ((rs = disp_start(&disp,pip,sip)) >= 0) {

	        rs = subinfo_reader(sip,&disp) ;
	        f = (rs < 0) ;

	        rs1 = disp_finish(&disp,f) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if */
#else /* CF_DISP */
	    rs = subinfo_reader(sip,&disp) ;
#endif /* CF_DISP */

/* finish up */

	    rs1 = subinfo_finish(sip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (subinfo) */

	return rs ;
}
/* end subroutine (procreaders) */

local int subinfo_start(SUBINFO *sip,PROGINFO *pip) noex {
	int		rs ;

	memclear(sip) ; /* dangerous */
	sip->pip = pip ;

	if ((rs = ids_load(&sip->id)) >= 0) {
	    cchar	*pr = pip->pr ;
	    if ((rs = hdrdecode_start(&sip->d,pr)) >= 0) {
	        ptm *dmp = &sip->dm ;
	        rs = dmp->create ;
	        if (rs < 0) {
	            hdrdecode_finish(&sip->d) ;
	        }
	    } /* end if (hdrdecode_start) */
	    if (rs < 0) {
	        ids_release(&sip->id) ;
	    }
	} /* end if (ids_load) */

	return rs ;
}
/* end subroutine (subinfo_start) */

local int subinfo_finish(SUBINFO *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    ptm *dmp = &sip->dm ;
	    rs1 = dmp->destroy ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = hdrdecode_finish(&sip->d) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = ids_release(&sip->id) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (subinfo_finish) */

local int subinfo_hdr(SUBINFO *sip,wchar_t *ibuf,int ilen,cc *sp,int sl) noex {
	ptm *dmp = &sip->dm ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if ((rs = dmp->lockbegin) >= 0) {
	    HDRDECODE	*hdp = &sip->d ;
	    {
	        rs = hdrdecode_proc(hdp,ibuf,ilen,sp,sl) ;
	        len = rs ;
	    }
	    rs1 = dmp->lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (mutex) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_hdr) */

local int subinfo_reader(SUBINFO *sip,DISP *dop) noex {
	PROGINFO	*pip = sip->pip ;
	POLLFD		fds[NFDS] ;
	time_t		ti_start = pip->daytime ;
	time_t		ti_termnote = pip->daytime ;
	time_t		ti_hangup = 0 ;
	cint	to_poll = TO_POLL ;
	cint	to = TO_READ ;	/* read time-out */
	cint	msglen = MSGBUFLEN ;
	cint	opts = FM_TIMED ;
	cint	fd_msg = pip->fd_msg ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		intrun ;
	int		intnote ;
	int		nfds ;
	int		f ;
	cchar	*pn = pip->progname ;
	cchar	*cp ;
	char		msgbuf[MSGBUFLEN+1] ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progcsreader/_reader: ent\n") ;
#endif

	intnote = pip->intnote ;
	intrun = ((pip->fl.daemon) ? pip->intrun : pip->intidle) ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4)) {
	    debugprintf("progcsreader/_reader: to_poll=%u to=%u\n",to_poll,to) ;
	    debugprintf("progcsreader/_reader: intrun=%d fd_msg=%d\n",
	        intrun,fd_msg) ;
	}
#endif

	nfds = 0 ;
	fds[nfds].fd = fd_msg ;
	fds[nfds].events = PG_POLLREG ;
	nfds += 1 ;
	fds[nfds].fd = -1 ;
	fds[nfds].events = 0 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progcsreader/_reader: 2 pn=%s\n",pip->progname) ;
#endif

	while (rs >= 0) {
	    int	pollwait = (to_poll*POLL_INTMULT) ;

#if	CF_DEBUG && 0
	    if (DEBUGLEVEL(4)) {
	        int	opts = FM_TIMED ;
	        rs1 = uc_recve(fd_msg,msgbuf,msglen,0,10,opts) ;
	        debugprintf("progcsreader/_reader: 3 pn=%s\n",
	            pip->progname) ;
	        debugprintf("progcsreader/_reader: PRE uc_recve() rs=%d\n",
	            rs1) ;
	        if (rs1 >= 0) {
	            debugprintf("progcsreader/_reader: PRE m=%>%r<\n",
	                msgbuf,strlinelen(msgbuf,rs1,30)) ;
	        }
	    }
#endif /* CF_DEBUG */

	    if ((rs = u_poll(fds,nfds,pollwait)) > 0) {
	        int	i ;
	        pip->daytime = time(nullptr) ;
	        for (i = 0 ; (rs >= 0) && (i < nfds) ; i += 1) {
	            cint	re = fds[i].revents ;
	            cint	fd = fds[i].fd ;
	            int		ml = 0 ;

#if	CF_DEBUG
	            if (DEBUGLEVEL(4)) {
	                char	ebuf[EBUFLEN+1] ;
	                debugprintf("progcsreader/_reader: 5 pn=%s\n",
	                    pip->progname) ;
	                debugprintf("progcsreader/_reader: poll re=%s\n",
	                    d_reventstr(re,ebuf,EBUFLEN)) ;
	            }
#endif /* CF_DEBUG */

	            if ((re & POLLIN) || (re & POLLPRI)) {

	                msgbuf[0] = '\0' ;
	                if (pip->fl.issocket) {
	                    rs = uc_recve(fd,msgbuf,msglen,0,to,opts) ;
	                    ml = rs ;
	                } else {
	                    rs = uc_reade(fd,msgbuf,msglen,to,opts) ;
	                    ml = rs ;
	                }

#if	CF_DEBUG
	                if (DEBUGLEVEL(4))
	                    debugprintf("progcsreader/_reader: 6 pn=%s\n",
	                        pip->progname) ;
#endif

	                if (pip->debuglevel > 0) {
	                    progeprintf(pip,"%s: recv(%d)\n",
	                        pip->progname,rs) ;
	                }

	                if ((rs >= 0) && (ml >= 0)) {

	                    if (ml && (msgbuf[ml-1] == '\n')) ml -= 1 ;
	                    while (ml && CHAR_ISWHITE(msgbuf[ml-1])) ml -= 1 ;

#if	CF_DEBUG
	                    if (DEBUGLEVEL(4)) {
	                        debugprintf("progcsreader/_reader: 7 pn=%s\n",
	                            pip->progname) ;
	                        debugprintf("progcsreader/_reader: m=>%r<\n",
	                            msgbuf,ml) ;
	                    }
#endif

	                    if (pip->debuglevel > 0) {
	                        progeprintf(pip,"%s: msg=>%r<\n",
	                            pn,msgbuf,strlinelen(msgbuf,ml,40)) ;
	                    }

	                    rs = subinfo_procmsg(sip,dop,msgbuf,ml) ;

#if	CF_DEBUG
	                    if (DEBUGLEVEL(4)) {
	                        debugprintf("progcsreader/_reader: 8 pn=%s\n",
	                            pip->progname) ;
	                        debugprintf("progcsreader/_reader: "
	                            "subinfo_procmsg() rs=%d\n", rs) ;
	                    }
#endif

	                } /* end if (had non-zero-length message) */

	                if (rs == SR_TIMEDOUT) rs = SR_OK ;

	            } else if (re & POLLNVAL) {
	                rs = SR_NOTOPEN ;
	            } else if (re & POLLERR) {
	                rs = SR_POLLERR ;
	            } else if (re & POLLHUP) {
	                if (ti_hangup == 0) ti_hangup = pip->daytime ;
	                msleep(200) ; /* wait for remaining data */
	            } /* end if (poll-events) */

	        } /* end for */
	    } else if (rs == SR_INTR) {
	        pip->daytime = time(nullptr) ;
	        rs = SR_OK ;
	    }

	    if ((rs >= 0) && (intrun > 0)) {
	        if ((pip->daytime - ti_start) >= intrun) break ;
	    }

	    if ((rs >= 0) && ti_hangup) {
	        if ((pip->daytime - ti_hangup) >= 1) break ;
	    }

	    if ((rs >= 0) && pip->open.logprog) {
#if	CF_DEBUG
	        if (DEBUGLEVEL(4))
	            debugprintf("progcsreader/_reader: "
	                "progloglock_maint()\n") ;
#endif
	        progloglock_maint(pip) ;
	    }

	    if ((rs >= 0) && ((pip->daytime - ti_termnote) >= intnote)) {
	        ti_termnote = pip->daytime ;
		ptm *tmp = &pip->tmutex ;
	        if ((rs = tmp->lockbegin) >= 0) {
		    {
	                rs = termnote_check(&pip->tn,pip->daytime) ;
		    }
	            tmp->lockend() ;
	        } /* end if (mutex-lock) */
	    } /* end if (termnote_check) */
	    if (rs >= 0) {
		rs = progexit(pip) ;
	    }
	} /* end while (looping) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progcsreader/_reader: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (subinfo_reader) */

local int subinfo_procmsg(SUBINFO *sip,DISP *dop,cchar *mbuf,int mlen) noex {
	PROGINFO	*pip = sip->pip ;
	int		rs = SR_OK ;

	if (mbuf[0] != '\0') {
	    int		ml = strnlen(mbuf,mlen) ;
	    cchar	*tp ;
	    cchar	*mp = mbuf ;

	    while ((tp = strnchr(mp,ml,'\n')) != nullptr) {

	        rs = disp_addwork(dop,mp,(tp-mp)) ;
	        pip->c_processed += 1 ;

	        ml -= ((tp+1)-mp) ;
	        mp = (tp+1) ;

	        if (rs < 0) break ;
	    } /* end while */

	    if ((rs >= 0) && (ml > 0)) {
	        rs = disp_addwork(dop,mp,ml) ;
	        pip->c_processed += 1 ;
	    }

	} /* end if (flie check) */

	return rs ;
}
/* end subroutine (subinfo_procmsg) */

local int disp_start(DISP *dop,PROGINFO *pip,SUBINFO *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (dop == nullptr) return SR_FAULT ;

	memclear(dop) ;
	dop->pip = pip ;
	dop->sip = sip ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("disp_start: pn=%s\n",pip->progname) ;
#endif

/* determine amount of parallelism */

	dop->n = pip->npar ;
	if (pip->npar == 0) {
	    rs = getnprocessors(pip->envv,0) ;
	    dop->n = (rs+1) ;
	}

	if (uptgetconcurrency() < dop->n) {
	    uptsetconcurrency(dop->n) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("progcsreader/disp_start: n=%u\n",dop->n) ;
#endif

/* continue */

	if (rs >= 0) {
	    if ((rs = fsi_start(&dop->wq)) >= 0) {
	        if ((rs = psem_create(&dop->wq_sem,FALSE,0)) >= 0) {
	            cint	psz = szof(pthread_t) ;
		    cint	vn = 10 ;
	            cint	vo = (VECOBJ_OREUSE) ;
	            if ((rs = vecobj_start(&dop->tids,psz,vn,vo)) >= 0) {
	                pthread_t	tid, *tidp ;
	                workthr		w = (workthr) disp_worker ;
	                int		i ;
/* create threads to handle it */
	                for (i = 0 ; (rs >= 0) && (i < dop->n) ; i += 1) {
	                    if ((rs = uptcreate(&tid,nullptr,w,dop)) >= 0) {
	                        rs = vecobj_add(&dop->tids,&tid) ;
	                    }
	                } /* end for */
/* all setup: but handle any errors */
	                if (rs < 0) {
	                    dop->f_exit = TRUE ;
	                    for (i = 0 ; i < dop->n ; i += 1) {
	                        psem_post(&dop->wq_sem) ;
	                    }
	                    i = 0 ;
	                    while (vecobj_get(&dop->tids,i,&tidp) >= 0) {
	                        if (tidp != nullptr) {
	                            uptjoin(*tidp,nullptr) ;
	                        }
	                        i += 1 ;
	                    } /* end while */
	                } /* end if (failure) */
	                if (rs < 0)
	                    vecobj_finish(&dop->tids) ;
	            } /* end if (vecobj) */
	            if (rs < 0)
	                psem_destroy(&dop->wq_sem) ;
	        } /* end if (ptm) */
	        if (rs < 0)
	            fsi_finish(&dop->wq) ;
	    } /* end if (fsi) */
	} /* end if (ok) */

	return rs ;
}
/* end subroutine (disp_start) */

local int disp_finish(DISP *dop,int f_abort) noex {
	PROGINFO	*pip ;
	pthread_t	*tidp ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		trs ;
	int		i ;

	if (dop == nullptr) return SR_FAULT ;

	pip = dop->pip ;
	dop->f_done = TRUE ;
	if (f_abort) dop->f_exit = TRUE ;

	for (i = 0 ; i < dop->n ; i += 1) {
	    psem_post(&dop->wq_sem) ;
	}

	for (i = 0 ; vecobj_get(&dop->tids,i,&tidp) >= 0 ; i += 1) {
	    if (tidp != nullptr) {
	        rs1 = uptjoin(*tidp,&trs) ;
	        if (rs >= 0) rs = rs1 ;
	        if (trs > 0) pip->c_updated += trs ;
	        if (rs >= 0) rs = trs ;
	    }
	} /* end for */

	rs1 = vecobj_finish(&dop->tids) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = psem_destroy(&dop->wq_sem) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = fsi_finish(&dop->wq) ;
	if (rs >= 0) rs = rs1 ;

	return rs ;
}
/* end subroutine (disp_finish) */

local int disp_addwork(DISP *dop,cchar *mbuf,int mlen) noex {
	int		rs ;

	if ((rs = fsi_add(&dop->wq,mbuf,mlen)) >= 0) {
	    rs = psem_post(&dop->wq_sem) ;
	}

	return rs ;
}
/* end subroutine (disp_addwork) */

/* of course this runs in parallel threads */
local int disp_worker(DISP *dop) noex {
	PROGINFO	*pip = dop->pip ;
	cint	mlen = MSGBUFLEN ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	char		mbuf[MSGBUFLEN+1] ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4)) {
	    pthread_t	tid = pthread_self() ;
	    debugprintf("progcsreader/worker: starting tid=%u\n",tid) ;
	    debugprintf("progcsreader/worker: pn=%s\n",pip->progname) ;
	}
#endif

	while ((rs >= 0) && (! dop->f_exit)) {
	    if ((rs = psem_wait(&dop->wq_sem)) >= 0) {
	        if ((rs = fsi_remove(&dop->wq,mbuf,mlen)) >= 0) {
	            cint	ml = rs ;
	            if (ml > 0) {
	                rs = progcsmsg(pip,mbuf,ml) ;
	                if (rs > 0) c += 1 ;
	            } /* end if (work to do) */
	        } else if (rs == SR_NOTFOUND) {
	            rs = SR_OK ;
	            if (dop->f_done) break ;
	        }
	    } else if (rs == SR_AGAIN) {
	        rs = SR_OK ;
	    }
	} /* end while (server loop) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4)) {
	    pthread_t	tid = pthread_self() ;
	    debugprintf("progcsreader/worker: tid=%u ret rs=%d c=%u\n",
	        tid,rs,c) ;
	}
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (disp_worker) */

#ifdef	COMMENT
local int ereport(PROGINFO *pip,cchar *fname,int frs) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (! pip->fl.quiet) {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt = "%s: file-processing error (%d)\n" ;
	    {
	    rs1 = progeprintf(pip,fmt,pn,frs) ;
	    if (rs >= 0) rs = rs1 ;
	    }
	    {
	    rs1 = progeprintf(pip,"%s: file=%s\n",pn,fname) ;
	    if (rs >= 0) rs = rs1 ;
	    }
	}
	return rs ;
}
/* end subroutine (ereport) */
#endif /* COMMENT */



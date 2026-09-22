/* uctimx SUPPORT (interval timer) */
/* charset=ISO8859-1 */
/* lang=C++11 */

/* interface components for UNIX® library-3c */
/* virtual per-process timer management */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */
#define	CF_CHILDTHRS	0		/* start threads in child process */

/* revision history:

	= 2014-04-04, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_timxcreate
	uc_timxdestroy
	uc_timxset
	uc_timxget
	uc_timxover

	Description:
	This module creates per-process (virtual) time-of-day timers
	for callers.  This interface (these subroutines) are meant
	to mimic the POSIX® real-time per-process timer facility.
	Why was this necessary?  Becuase some (stupid) operating
	systems which will not be named but have the initials --
	Apple Darwin -- do not have the POSIX® rea-time per-process
	timers.  Just a note: unlinke the POSIX® real-time per-process
	timers, this favility only uses a timer resolution of
	microseconds rather than nanoseconds (which the POSIX®
	interface uses).

	Synopsis:
	typedef ITIMEVAL	itim
	typedef CITIMEVAL	citim
	int uc_timxcreate	(con uctimxent *notep) noex
	int uc_timxdestroy	(int id) noex
	int uc_timxset		(int id,mut time_t *rtp,time_t ntim) noex
	int uc_timxget		(int id,mut time_t *rtp) noex
	int uc_timxover		(int id) noex

	Arguments:
	notep		UCTIM object pointer
	id		timer identification
	rtp		result time pointer
	nt		new time

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX */
#include	<sys/time.h>		/* POSIX <- interval timers are here */
#include	<pthread.h>		/* POSIX |PTHREAD_SCOPE_SYSTEM| */
#include	<ucontext.h>		/* POSIX */
#include	<ctime>			/* CSTD i-timer types */
#include	<csignal>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD placement-new */
#include	<memory>		/* C++STD |destroy_at(3c++)| */
#include	<numeric>		/* C++STD |cast_saturate(3c++)| */
#include	<queue>			/* C++STD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<timewatch.hh>		/* LIBU */
#include	<itimers.hh>		/* LIBU i-timer selection */
#include	<timeval.hh>		/* LIBU */
#include	<itimerval.h>		/* LIBU */
#include	<ptm.h>			/* LIBU */
#include	<ptc.h>			/* LIBU */
#include	<pta.h>			/* LIBU */
#include	<upt.h>			/* LIBU */
#include	<intsat.h>		/* LIBU */
#include	<ucmpx.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucatexit.h>		/* LIBUC */
#include	<ucatfork.h>		/* LIBUC */
#include	<ucsigset.h>		/* LIBUC */
#include	<vechand.h>		/* LIBUC vector-handles */
#include	<vecsorthand.h>		/* LIBUC vector-sorted-handles */
#include	<ciq.h>			/* LIBUC container-interlocked-queue */
#include	<sigevent.h>		/* LIBUC */
#include	<psem.h>		/* LIBUC POSIX® semaphore */
#include	<localmisc.h>		/* LIBU */

#include	"uctimx.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */
import usigsets ;			/* |usigset(3u)| */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif
#ifndef	CF_CHILDTHRS
#define	CF_CHILDTHRS	0
#endif

#define	UCTIM_SCOPE	PTHREAD_SCOPE_SYSTEM

#define	TO_CAPTURE	60		/* timeout: capture wait for threads */
#define	TO_SIGWAIT	2		/* timeout: signal-process wait */
#define	TO_DISPRECV	5		/* timeout: dispatch-process wait */


/* imported namespaces */

using std::cast_saturate ;		/* subroutine */
using std::destroy_at ;			/* subroutine */
using std::min ;			/* subroutine */
using std::max ;			/* subroutine */
using libu::uitimer_get ;		/* subroutine */
using libu::uitimer_set ;		/* subroutine */


/* local typedefs */

extern "C" {
    typedef int (*tworker_f)(void *) noex ;
} /* end extern (C) */

typedef vecsorthand	prique ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct uctimxent {
	uctimx_f	notf ;		/* notify function (C-linkage) */
	void		*objp ;		/* object pointer (function argument) */
	psem		*psemp ;	/* POSIX® Semaphore pointer */
	time_t		val ;		/* i-timer-value */
	int		id ;		/* timer-ID */
	int		notarg ;	/* notification function argument */
    } ; /* end struct (uctimxent) */
} /* end namespace */

enum dispcmds {
	dispcmd_exit,
	dispcmd_timeout,
	dispcmd_handle,
	dispcmd_overlast
} ; /* end enum */

enum cmdsubs {
	cmdsub_create,
	cmdsub_destroy,
	cmdsub_set,
	cmdsub_get,
	cmdsub_over,
	cmdsub_overlast
} ; /* end enum */

namespace {
    enum mimemgrmems {
	timemgrmem_init,
	timemgrmem_fini,
	timemgrmem_capbeg,
	timemgrmem_capend,
	timemgrmem_ovelast
    } ; /* end enum (timemgrmems) */
    struct timemgr ;
    struct timemgr_arg {
	con uctimxnote	*notep ;
	time_t		*rtp ;		/* remaining-time-pointer */
	time_t		ntim ;		/* new-time */
	timemgr_arg() noex : notep(nullptr), rtp(nullptr), ntim(0L) { } ;
	timemgr_arg(con uctimxnote *c) noex : notep(c) { } ;
	timemgr_arg(time_t *p,time_t n = 0) noex : rtp(p), ntim(n) { } ;
	int operator () (cmdsubs,int = 0) noex ;
    } ; /* end struct (timemgr_arg) */
    struct timemgr_fl {
	uint		timer:1 ;	/* UNIX®-RT timer created */
	uint		workready:1 ;
	uint		thrs:1 ;
	uint		wasblocked:1 ;
	uint		running_siger:1 ;
	uint		running_disper:1 ;
    } ; /* end struct (timemgr_fl) */
    struct timemgr_co {
	timemgr		*op = nullptr ;
	int		w = -1 ;
	void operator () (timemgr *p,int m) noex {
	    op = p ;
	    w = m ;
	} ; /* end */
	int operator () (int = -1) noex ;
	operator int () noex {
	    return operator () () ;
	} ; /* end */
    } ; /* end struct (timemgr_co) */
    struct timemgr {
	friend		timemgr_co ;
	timemgr_co	init ;
	timemgr_co	fini ;
	timemgr_co	capbeg ;
	timemgr_co	capend ;
	ptm		mtx ;		/* data mutex */
	ptc		cnv ;		/* condition variable */
	vechand		ents ;
	ciq		pass ;
	prique		*pqp ;
	sigset_t	savemask ;
	pthread_t	tid_siger ;
	pthread_t	tid_disper ;
	timemgr_fl	fl ;
	vol int		waiters ;	/* n-waiters for general capture */
	aflag		fvoid ;
	aflag		finit ;
	aflag		finitdone ;
	aflag		fcapture ;	/* capture flag */
	aflag		fthrsiger ;	/* thread running (siger) */
	aflag		fthrdisp ;	/* thread running (disp) */
	aflag		fcmd ;
	aflag		freqexit ;	/* request exit of threads */
	aflag		fexitsiger ;	/* thread is exiting */
	aflag		fexitdisp ;	/* thread is exiting */
	timemgr() noex {
	    init	(this,timemgrmem_init) ;
	    fini	(this,timemgrmem_fini) ;
	    capbeg	(this,timemgrmem_capbeg) ;
	    capend	(this,timemgrmem_capend) ;
	} ; /* end ctor */
	int cmd_create	(int,timemgr_arg *) noex ;
	int cmd_destroy	(int,timemgr_arg *) noex ;
	int cmd_set	(int,timemgr_arg *) noex ;
	int cmd_get	(int,timemgr_arg *) noex ;
	int cmd_over	(int,timemgr_arg *) noex ;
	int cmdsub	(cmdsubs,int,timemgr_arg *) noex ;
	int priqins	(uctimxent *) noex ;
	int priqrem	(uctimxent *) noex ;
	int timerset	(time_t) noex ;
	int workready	() noex ;
	int workbegin	() noex ;
	int workend	() noex ;
	int entfins	() noex ;
	int workdump	() noex ;
	int priqbegin	() noex ;
	int priqend	() noex ;
	int pridump	() noex ;
	int sigbegin	() noex ;
	int sigend	() noex ;
	int timerbegin	() noex ;
	int timerend	() noex ;
	int thrsbegin	() noex ;
	int thrsend	() noex ;
	int sigerbeg	() noex ;
	int sigerend	() noex ;
	int sigerwork	() noex ;
	int sigerwait	() noex ;
	int sigerserve	() noex ;
	int sigertrans	(int,uctimxent *) noex ;
	int sigerdump	() noex ;
	int dispbeg	() noex ;
	int dispend	() noex ;
	int dispworker	() noex ;
	int disprecv	() noex ;
	int disphandle	() noex ;
	int dispdeliver	(uctimxent *) noex ;
	int disprempri	(uctimxent *) noex ;
	int deliver	(uctimxent *) noex ;
	int deliversem	(uctimxent *) noex ;
#ifdef	COMMENT
	int dispjobdel	(uctimxent *) noex ;
#endif
	destruct timemgr() noex {
	    if (cint rs = fini ; rs < 0) {
		ulogerror("timemgr",rs,"dtor-fini") ;
	    }
	} ; /* end dtor (timemgr) */
    private:
	int pinit	() noex ;
	int pfini	() noex ;
	int pcapbeg	(int) noex ;
	int pcapend	() noex ;
    } ; /* end struct (timemgr) */
} /* end namespace */


/* forward references */

local void uctimxent_load(uctimxent *ep,con uctimxnote *nop) noex ;

local int	cmpqent(cvoid *,cvoid *) noex ;

extern "C" {
    local int	timemgr_sigerwork(timemgr *) noex ;
    local int	timemgr_dispworker(timemgr *) noex ;
    local void	timemgr_atforkbefore() noex ;
    local void	timemgr_atforkparent() noex ;
    local void	timemgr_atforkchild() noex ;
    local void	timemgr_exit() noex ;
} /* end extern */


/* local variables */

static timemgr		timemgr_data ;
cint			itimid		= itimer.real ;
cint			sigto		= SIGALARM ;
cbool			f_childthrs	= CF_CHILDTHRS ;


/* exported variables */


/* exported subroutines */

int uc_timxcreate(con uctimxnote *notep) noex {
	timemgr_arg	ao(notep) ;
	return ao(cmdsub_create) ;
} /* end subroutine */

int uc_timxdestroy(int id) noex {
	timemgr_arg	ao ;
	return ao(cmdsub_destroy,id) ;
} /* end subroutine */

int uc_timxset(int id,time_t *rtp,time_t ntim) noex {
	timemgr_arg	ao(rtp,ntim) ;
	return ao(cmdsub_set,id) ;
} /* end subroutine */

int uc_timxget(int id,time_t *rtp) noex {
	timemgr_arg	ao(rtp) ;
	return ao(cmdsub_get,id) ;
} /* end subroutine */

int uc_timxover(int id) noex {
	timemgr_arg	ao ;
	return ao(cmdsub_over,id) ;
} /* end subroutine */


/* local subroutines */

int timemgr_arg::operator () (cmdsubs cmd,int id) noex {
	return timemgr_data.cmdsub(cmd,id,this) ;
} /* end method (timemgr_arg::operator) */

int timemgr::cmdsub(cmdsubs cmd,int id,timemgr_arg *uap) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rv = 0 ;
	if (uap) ylikely {
	    rs = SR_INVALID ;
	    if (cmd >= 0) ylikely {
	        if ((rs = init) >= 0) ylikely {
	            if ((rs = capbeg) >= 0) ylikely {
	                if ((rs = workready()) >= 0) ylikely {
	                    switch (cmd) {
	                    case cmdsub_create:
	                        rs = cmd_create		(id,uap) ;
	                        break ;
	                    case cmdsub_destroy:
	                        rs = cmd_destroy	(id,uap) ;
	                        break ;
	                    case cmdsub_set:
	                        rs = cmd_set		(id,uap) ;
	                        break ;
	                    case cmdsub_get:
	                        rs = cmd_get		(id,uap) ;
	                        break ;
	                    case cmdsub_over:
	                        rs = cmd_over		(id,uap) ;
	                        break ;
	                    default:
	                        rs = SR_INVALID ;
	                        break ;
	                    } /* end switch */
			    rv = rs ;
	                } /* end if (timemgr_workready) */
	                rs1 = capend ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (uctimx-cap) */
	        } /* end if (timemgr_init) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rv : rs ;
} /* end method (timemgr::cmdsub) */

int timemgr::pinit() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (! fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    rs = SR_OK ;
	    if (! finit.testandset) {
	        if ((rs = mtx.create) >= 0) ylikely {
	            if ((rs = cnv.create) >= 0) ylikely {
	                void_f	b = timemgr_atforkbefore ;
	                void_f	ap = timemgr_atforkparent ;
	                void_f	ac = timemgr_atforkchild ;
	                if ((rs = uc_atforkrec(b,ap,ac)) >= 0) ylikely {
			    void_f	e = timemgr_exit ;
	                    if ((rs = uc_atexit(e)) >= 0) ylikely {
	                        finitdone = true ;
	                        f = true ;
	                    } /* end if (ready) */
	                    if (rs < 0) {
	                        uc_atforkexp(b,ap,ac) ;
			    } /* end if (error) */
	                } /* end if (uc_atfork) */
	                if (rs < 0) {
	                    cnv.destroy() ;
			} /* end if (error) */
	            } /* end if (ptc::create) */
	            if (rs < 0) {
	                mtx.destroy() ;
		    } /* end if (error) */
	        } /* end if (ptm::create) */
	        if (rs < 0) {
	            finit = false ;
		} /* end if (error) */
	    } else if (! finitdone) {
	        timewatch	tw(to) ;
	        auto lamb = [this] () -> int {
	            int		rsl = SR_OK ;
	            if (!finit) ylikely {
		        rsl = SR_LOCKFAIL ;
	            } else if (finitdone) {
		        rsl = 1 ;
	            }
	            return rsl ;
	        } ; /* end lambda */
	        rs = tw(lamb) ;			/* <- time-watching */
	    } /* end if (initialization) */
	} /* end if (not-voided) */
	return (rs >= 0) ? f : rs ;
} /* end method (timemgr::pinit) */

int timemgr::pfini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone && (! fvoid.testandset)) {
	    {
	        rs1 = workend() ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        void_f	b = timemgr_atforkbefore ;
	        void_f	ap = timemgr_atforkparent ;
	        void_f	ac = timemgr_atforkchild ;
	        rs1 = uc_atforkexp(b,ap,ac) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = cnv.destroy ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = mtx.destroy ;
		if (rs >= 0) rs = rs1 ;
	    }
	    finit = false ;
	    finitdone = false ;
	} /* end if (was initialized) */
	return rs ;
} /* end method (timemgr::pfini) */

int timemgr::pcapbeg(int to) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mtx.lockbegin(to)) >= 0) {
	    waiters += 1 ;
	    while ((rs >= 0) && fcapture) { /* busy */
	        rs = cnv.wait(&mtx,to) ;
	    } /* end while */
	    if (rs >= 0) {
	        fcapture = true ;
	    } /* end if (ok) */
	    waiters -= 1 ;
	    rs1 = mtx.lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return rs ;
} /* end method (timemgr::pcapbeg) */

int timemgr::pcapend() noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mtx.lockbegin) >= 0) {
	    fcapture = false ;
	    if (waiters > 0) {
	        rs = cnv.signal ;
	    }
	    rs1 = mtx.lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return rs ;
} /* end method (timemgr::pcapend) */

int timemgr::cmd_create(int,timemgr_arg *argp) noex {
    	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (argp->notep) ylikely {
	    cint	esz = szof(uctimxent) ;
	    if (void *vp ; (rs = lm_mall(esz,&vp)) >= 0) ylikely {
		rs = SR_BUGCHECK ;
	        if (uctimxent *ep = new(vp) uctimxent ; ep) ylikely {
		    uctimxent_load(ep,argp->notep) ;
	            if ((rs = ents.add(ep)) >= 0) ylikely {
	                ep->id = rs ;
	            } /* end if (vechand_add) */
		    if (rs < 0) {
			destroy_at(ep) ;
		    } /* end if (error) */
	        } /* end if (new-uctimxent) */
	        if (rs < 0) ylikely {
	            lm_free(vp) ;
		} /* end if (error) */
	    } /* end if (memory-acquire) */
	} /* end if (non-null) */
	return rs ;
} /* end method (timemgr::cmd_create) */

int timemgr::cmd_destroy(int id,timemgr_arg *) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	if (void *vp ; (rs = ents.get(id,&vp)) >= 0) ylikely {
	    cint	ei = rs ;
	    rs = SR_BUGCHECK ;
	    if (uctimxent *ep = resumelife<uctimxent>(vp) ; ep) ylikely {
	        if ((rs = ents.del(ei)) >= 0) ylikely {
		    bool	f_free = false ;
	            if ((rs = pqp->delhand(ep)) >= 0) ylikely {
			f_free = true ;
		    } else if (rs == rsn) {
	                if ((rs = pass.rement(ep)) >= 0) {
			    f_free = true ;
			} else if (rs == rsn) {
	                    rs = SR_OK ;
	                }
		    } /* end if */
		    if ((rs >= 0) && f_free) {
			destroy_at(ep) ;
		    } /* end if (destroy_at) */
		    if ((rs >= 0) && f_free) {
	            	rs1 = lm_free(ep) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (memory-release) */
	        } /* end if (vechand_del) */
	    } /* end if (non-null) */
	} /* end if (vechand_get) */
	return rs ;
} /* end method (timemgr::cmd_destroy) */

int timemgr::cmd_set(int id,timemgr_arg *uap) noex {
	int		rs ;
	(void) uap ;
	if (void *vp ; (rs = ents.get(id,&vp)) >= 0) ylikely {
	    if (uctimxent *ep = resumelife<uctimxent>(vp) ; ep) ylikely {
		custime dt = time(nullptr) ;
		if (time_t *rtp = uap->rtp) {
		    *rtp = max((ep->val - dt),0L) ;
		} /* end if (remaining time) */
		rs = priqins(ep) ;
	    } /* end if (non-null) */
	} /* end if (vechand_get) */
	return rs ;
} /* end method (timemgr::cmd_set) */

int timemgr::cmd_get(int id,timemgr_arg *) noex {
    	int		rs = SR_OK ;
	(void) id ;
	return rs ;
} /* end method (timemgr::cmd_get) */

int timemgr::cmd_over(int id,timemgr_arg *) noex {
    	int		rs = SR_OK ;
	(void) id ;
	return rs ;
} /* end method (timemgr::cmd_over) */

int timemgr::priqins(uctimxent *ep) noex {
	int		rs ;
	int		pi = 0 ;
	if ((rs = pqp->count) > 0) {
	    if (uctimxent *tep ; (rs = pqp->get(0,&tep)) >= 0) {
	        if (ep->val < tep->val) {
	            if ((rs = pqp->add(ep)) >= 0) {
	                pi = rs ;
	                rs = timerset(ep->val) ;
	                if (rs < 0) {
	                    pqp->del(pi) ;
			} /* end if (error) */
	            } /* end if (add) */
	        } else {
	            rs = pqp->add(ep) ;
	            pi = rs ;
	        } /* end */
	    } /* end if (vecsorthand_get) */
	} else {
	    if ((rs = pqp->add(ep)) >= 0) {
	        pi = rs ;
	        rs = timerset(ep->val) ;
	        if (rs < 0) {
	            pqp->del(pi) ;
		} /* end if (error) */
	    } /* end if (vecsorthand_add) */
	} /* end if */
	return (rs >= 0) ? pi : rs ;
} /* end method (timemgr::priqins) */

int timemgr::priqrem(uctimxent *ep) noex {
    	cint		rsn = SR_NOTFOUND ;
    	int		rs ;
	if ((rs = pqp->delhand(ep)) == rsn) {
	    rs = SR_OK ;
	}
	return rs ;
} /* end method (timemgr::priqrem) */

int timemgr::timerset(time_t val) noex {
    	cnullptr	np{} ;
	int		rs ;
	if (TIMEVAL tv ; (rs = timeval_load(&tv,val,0)) >= 0) ylikely {
	    if (ITIMERVAL it ; (rs = itimerval_load(&it,&tv,np)) >= 0) {
	        rs = uitimer_set(itimid,&it,np) ;
	    } /* end if (ITIMETVAL) */
	} /* end if (TIMEVVAL) */
	return rs ;
} /* end method (timemgr::timerset) */

int timemgr::workready() noex {
	int		rs = SR_OK ;
	if (! fl.workready) {
	    rs = workbegin() ;
	} /* end if (work) */
	if ((rs >= 0) && (! fl.thrs)) {
	    rs = thrsbegin() ;
	} /* end if (threads) */
	return rs ;
} /* end method (timemgr::workready) */

int timemgr::workbegin() noex {
	int		rs = SR_OK ;
	if (! fl.workready) {
	    cint	vn = 0 ;
	    cint	vo = (vechandm.compact | vechandm.ordered) ;
	    if ((rs = ents.start(vn,vo)) >= 0) ylikely {
	        if ((rs = priqbegin()) >= 0) ylikely {
	            if ((rs = sigbegin()) >= 0) ylikely {
	                if ((rs = timerbegin()) >= 0) ylikely {
	                    if ((rs = pass.start) >= 0) ylikely {
	                        if ((rs = thrsbegin()) >= 0) {
	                            fl.workready = true ;
	                        } /* end if (good-to-go) */
	                        if (rs < 0) {
	                            pass.finish() ;
	                        } /* end if (error) */
	                    } /* end if (ciq_start) */
	                    if (rs < 0) {
	                        timerend() ;
	                    } /* end if (error) */
	                } /* end if (timemgr::timerbegin) */
	                if (rs < 0) {
	                    sigend() ;
			} /* end if (error) */
	            } /* end if (timemgr::sigbegin) */
	            if (rs < 0) {
	                priqend() ;
	            } /* end if (error) */
	        } /* end if (timemgr::pribegin) */
	        if (rs < 0) {
	            ents.finish() ;
	        } /* end if (error) */
	    } /* end if (vechand_start) */
	} /* end if (needed) */
	return rs ;
} /* end method (timemgr::workbegin) */

int timemgr::workend() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (fl.workready) {
	    {
	        rs1 = thrsend() ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = pass.finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = timerend() ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = sigend() ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = priqend() ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = entfins() ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ents.finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    fl.workready = false ;
	} /* end if (work-ready) */
	return rs ;
} /* end method (timemgr::workend) */

int timemgr::entfins() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	void		*otp{} ;
	for (int i = 0 ; ents.get(i,&otp) >= 0 ; i += 1) {
	    if (otp) {
	        if (uctimxent *ep = resumelife<uctimxent>(otp) ; ep) {
		    destroy_at(ep) ;
		}
		{
	            rs1 = lm_free(otp) ;
	            if (rs >= 0) rs = rs1 ;
		}
	    } /* end if (memory-release) */
	} /* end for */
	return rs ;
} /* end method (timemgr_entfins) */

int timemgr::workdump() noex {
        int             rs = SR_OK ;
        int             rs1 ;
        if (fl.workready) {
	    {
                rs1 = pridump() ;
                if (rs >= 0) rs = rs1 ;
	    }
	    {
                rs1 = sigerdump() ;
                if (rs >= 0) rs = rs1 ;
	    }
	    {
                rs1 = entfins() ;
                if (rs >= 0) rs = rs1 ;
	    }
        } /* end if (work-ready) */
        return rs ;
} /* end method (timemgr::workdump) */

int timemgr::priqbegin() noex {
	cint		osz = szof(vecsorthand) ;
	int		rs ;
	if (void *p ; (rs = lm_mall(osz,&p)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (pqp = new(p) prique ; pqp) {
	        rs = pqp->start(cmpqent,1) ;
		if (rs < 0) {
		    destroy_at(pqp) ;
		} /* end if (error) */
	    } /* end if (construct-prique) */
	    if (rs < 0) {
	        lm_free(pqp) ;
	        pqp = nullptr ;
	    } /* end if (error) */
	} /* end if (memory-acquire) */
	return rs ;
} /* end subroutine (timemgr::priqbegin) */

int timemgr::priqend() noex {
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (pqp) {
	    rs = SR_OK ;
	    {
	        rs1 = pqp->finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		destroy_at(pqp) ;
	    }
	    {
	        rs1 = lm_free(pqp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (memory-release) */
	    pqp = nullptr ;
	} /* end if (non-null) */
	return rs ;
} /* end method (timemgr::priqend) */

int timemgr::pridump() noex {
        int             rs = SR_OK ;
        int             rs1 ;
        void            *tep ;
        for (int i = 0 ; (rs1 = pqp->get(i,&tep)) >= 0 ; i += 1) {
            if (tep) {
                rs1 = pqp->del(i--) ;
                if (rs >= 0) rs = rs1 ;
            }
        } /* end for */
        if ((rs >= 0) && (rs1 != SR_NOTFOUND)) rs = rs1 ;
        return rs ;
} /* end method (timemgr::pridump) */

int timemgr::sigbegin() noex {
	usigset		nss(sigto) ;
	usigset		oss ;
	cint		scmd = SIG_BLOCK ; /* <- block */
	int		rs ;
	if ((rs = u_sigmask(scmd,&nss,&oss)) >= 0) {
	    if ((rs = oss.is(sigto)) > 0) {
	        fl.wasblocked = true ;
	    }
	} /* end if (u_sigmask) */
	return rs ;
} /* end method (timemgr::sigbegin) */

int timemgr::sigend() noex {
	int		rs = SR_OK ;
	if (! fl.wasblocked) {
	    usigset	nsm(sigto) ;
	    cint	scmd = SIG_UNBLOCK ; /* <- un-block */
	    rs = u_sigmask(scmd,&nsm) ;
	} /* end if (was blocked) */
	return rs ;
} /* end method (timemgr::sigend) */

int timemgr::timerbegin() noex {
	fl.timer = true ;
	return SR_OK ;
} /* end method (timemgr::timerbegin) */

int timemgr::timerend() noex {
	fl.timer = false ;
	return SR_OK ;
} /* end method (timemgr::timerend) */

int timemgr::thrsbegin() noex {
	int		rs = SR_OK ;
	if ((! fl.thrs) && (! freqexit)) {
	    if ((rs = sigerbeg()) >= 0) {
	        if ((rs = dispbeg()) >= 0) {
	            fl.thrs = true ;
	        }
	        if (rs < 0) {
	            sigerend() ;
	        } /* end if (error) */
	    } /* end if (sigerbeg) */
	} /* end if (needed) */
	return rs ;
} /* end method (timemgr::thrsbegin) */

int timemgr::thrsend() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (fl.thrs) {
	    fl.thrs = false ;
	    freqexit = true ;
	    {
	        rs1 = dispend() ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = sigerend() ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if */
	return rs ;
} /* end method (timemgr::thrsend) */

int timemgr::sigerbeg() noex {
	int		rs ;
	int		rs1 ;
	int		f = false ;
	if (pta ta ; (rs = ta.create) >= 0) ylikely {
	    cint	scope = UCTIM_SCOPE ;
	    if ((rs = ta.setscope(scope)) >= 0) ylikely {
	        tworker_f	wt = tworker_f(timemgr_sigerwork) ;
	        if (pthread_t tid ; (rs = uptcreate(&tid,&ta,wt,this)) >= 0) {
	            fl.running_siger = true ;
	            tid_siger = tid ;
	            f = true ;
	        } /* end if (pthread-create) */
	    } /* end if (pta-setscope) */
	    rs1 = ta.destroy ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (pta) */
	return (rs >= 0) ? f : rs ;
} /* end method (timemgr_sigerbeg) */

int timemgr::sigerend() noex {
	int		rs = SR_OK ;
	if (fl.running_siger) {
	    pthread_t	tid = tid_siger ;
	    if ((rs = uptkill(tid,sigto)) >= 0) {
	        fl.running_siger = false ;
	        if (int trs ; (rs = uptjoin(tid,&trs)) >= 0) {
	            rs = trs ;
	        } else if (rs == SR_SRCH) {
	            rs = SR_OK ;
	        }
	    } /* end if (uptkill) */
	} /* end if (running) */
	return rs ;
} /* end method (timemgr_sigerend) */

/* this is an independent thread of execution */
int timemgr::sigerwork() noex {
	int		rs ;
	while ((rs = sigerwait()) > 0) {
	    if (freqexit) break ;
	    switch (rs) {
	    case dispcmd_timeout:
	        rs = sigerserve() ;
	        break ;
	    } /* end switch */
	    if (rs < 0) break ;
	} /* end while */
	fexitsiger = true ;
	return rs ;
} /* end method (timemgr::sigerwork) */

int timemgr::sigerwait() noex {
	int		rs ;
	int		cmd = 0 ; /* return-value */
	if (usigset nss(sigto) ; (rs = u_sigwait(&nss)) >= 0) {
	    if (rs == sigto) {
	        cmd = dispcmd_timeout ;
	    } else {
	        cmd = dispcmd_handle ;
	    } /* end */
	} /* end if (u_sigwait) */
	return (rs >= 0) ? cmd : rs ;
} /* end method (timemgr::sigerwait) */

int timemgr::sigerserve() noex {
	cint		to = TO_CAPTURE ;
	int		rs ;
	int		rs1 ;
	if ((rs = capbeg(to)) >= 0) ylikely {
	    custime	dt = time(nullptr) ;
	    while ((rs = pqp->count) > 0) {
	        if (uctimxent *tep ; (rs = pqp->get(0,&tep)) >= 0) {
	            cint ei = rs ; 
		    if (tep->val < dt) break ;
		    rs = sigertrans(ei,tep) ;
	        } /* end if (prique-get) */
	        if (rs < 0) break ;
	    } /* end while */
	    rs1 = capend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (capture) */
	return rs ;
} /* end method (timemgr::sigerserve) */

int timemgr::sigertrans(int ei,uctimxent *tep) noex {
    	int		rs ;
	if ((rs = pqp->del(ei)) >= 0) {
	    if ((rs = pass.ins(tep)) >= 0) {
		fcmd = true ;
		rs = cnv.signal ;
	    }
	} /* end if (delete) */
	return rs ;
} /* end method (timemgr::sigertrans) */

int timemgr::sigerdump() noex {
        int             rs = SR_OK ;
        int             rs1 ;
        void            *tep ;
        while ((rs1 = pass.rem(&tep)) >= 0) ; /* loop */
        if ((rs >= 0) && (rs1 != SR_NOTFOUND)) rs = rs1 ;
        return rs ;
} /* end method (timemgr::sigerdump) */

int timemgr::dispbeg() noex {
	int		rs ;
	int		rs1 ;
	int		f = false ;
	if (pta ta ; (rs = ta.create) >= 0) ylikely {
	    cint	scope = UCTIM_SCOPE ;
	    if ((rs = ta.setscope(scope)) >= 0) ylikely {
	        tworker_f	wt = tworker_f(timemgr_dispworker) ;
	        if (pthread_t tid ; (rs = uptcreate(&tid,&ta,wt,this)) >= 0) {
	            fl.running_disper = true ;
	            tid_disper = tid ;
	            f = true ;
	        } /* end if (uptcreate) */
	    } /* end if (pta-setscope) */
	    rs1 = ta.destroy ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (pta) */
	return (rs >= 0) ? f : rs ;
} /* end method (timemgr::dispbeg) */

int timemgr::dispend() noex {
	int		rs = SR_OK ;
	if (fl.running_disper) {
	    pthread_t	tid = tid_disper ;
	    fl.running_disper = false ;
	    if (int trs{} ; (rs = uptjoin(tid,&trs)) >= 0) {
	        rs = trs ;
	    } else if (rs == SR_SRCH) {
	        rs = SR_OK ;
	    }
	} /* end if (running) */
	return rs ;
} /* end method (timemgr::dispend) */

/* it always takes a good bit of code to make this part look easy! */
int timemgr::dispworker() noex {
	int		rs ;
	while ((rs = disprecv()) > 0) {
	    switch (rs) {
	    case dispcmd_timeout:
	        break ;
	    case dispcmd_handle:
	        rs = disphandle() ;
	        break ;
	    } /* end switch */
	    if (rs < 0) break ;
	} /* end while (looping on commands) */
	fexitdisp = true ;
	return rs ;
} /* end method (timemgr::dispworker) */

int timemgr::disprecv() noex {
	cint		to = TO_DISPRECV ;
	int		rs ;
	int		rs1 ;
	int		cmd = dispcmd_exit ;
	if ((rs = mtx.lockbegin) >= 0) ylikely {
	    waiters += 1 ;
	    while ((rs >= 0) && (! fcmd)) {
	        rs = cnv.wait(&mtx,to) ;
	    } /* end while */
	    if (rs >= 0) {
	        fcmd = false ;
	        if (! freqexit) cmd = dispcmd_handle ;
	        if (waiters > 1) {
	            rs = cnv.signal ;
	        }
	    } else if (rs == SR_TIMEDOUT) {
	        if (! freqexit) cmd = dispcmd_timeout ;
	        rs = SR_OK ;
	    } /* end if */
	    waiters -= 1 ;
	    rs1 = mtx.lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (mutex-section) */
	return (rs >= 0) ? cmd : rs ;
} /* end method (timemgr::disprecv) */

int timemgr::disphandle() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	for (uctimxent *tep ; (rs1 = pass.rem(&tep)) >= 0 ; ) {
	    rs = dispdeliver(tep) ;
	    if (rs < 0) break ;
	} /* end while */
	if ((rs >= 0) && (rs1 != SR_EMPTY)) rs = rs1 ;
	return rs ;
} /* end method (timemgr::disphandle) */

int timemgr::dispdeliver(uctimxent *tep) noex {
    	int		rs ;
	if ((rs = disprempri(tep)) >= 0) {
	    rs = deliver(tep) ;
	} /* end if (disprempri) */
	return rs ;
} /* end method (timemgr::dispdeliver) */

int timemgr::disprempri(uctimxent *tep) noex {
        cint       	to = TO_CAPTURE ;
    	int		rs = SR_OK ;
	int		rs1 ;
        if ((rs = capbeg(to)) >= 0) ylikely {
	    {
		rs = priqrem(tep) ;
	    }
	    rs1 = capend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (capture) */
	return rs ;
} /* end method (timemgr::disprempri) */

int timemgr::deliver(uctimxent *tep) noex {
    	int		rs ;
	if ((rs = deliversem(tep)) > 0) {
	    if (cauto notf = tep->notf) {
	        rs = notf(tep->objp,tep->id,tep->notarg) ;
	    } /* end if */
	} /* end if (deliversem) */
	return rs ;
} /* end method (timemgr::deliver) */

int timemgr::deliversem(uctimxent *tep) noex {
    	int		rs = SR_OK ;
	if (psem *psp = tep->psemp) {
	    rs = psp->post ;
	} /* end if (had semaphore) */
	return rs ;
} /* end method (timemgr::deliversem) */

#ifdef	COMMENT
int timemgr::dispjobdel(uctimxent *tep) noex {
        cint       	to = TO_CAPTURE ;
	int		rs ;
	int		rs1 ;
	int		f = false ; /* return-value */
        if ((rs = capbeg(to)) >= 0) ylikely {
	    if ((rs = ents.delhand(tep)) >= 0) {
		f = true ;
	    } else if (rs == SR_NOTFOUND) {
		rs = SR_OK ;
	    }
	    rs1 = capend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (uctimx-cap) */
	return (rs >= 0) ? f : rs ;
} /* end method (timemgr::dispjobdel) */
#endif /* COMMENT */

int timemgr_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case timemgrmem_init:
	        rs = op->pinit() ;
	        break ;
	    case timemgrmem_fini:
	        rs = op->pfini() ;
	        break ;
	    case timemgrmem_capbeg:
	        rs = op->pcapbeg(a) ;
	        break ;
	    case timemgrmem_capend:
	        rs = op->pcapend() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (timemgr_co::operator) */

local int timemgr_sigerwork(timemgr *tmp) noex {
	return tmp->sigerwork() ;
} /* end subroutine */

local int timemgr_dispworker(timemgr *tmp) noex {
	return tmp->dispworker() ;
} /* end subroutine */

local void timemgr_atforkbefore() noex {
	timemgr_data.mtx.lockbegin() ;
} /* end subroutine (timemgr_atforkbefore) */

local void timemgr_atforkparent() noex {
	timemgr_data.mtx.lockend() ;
} /* end subroutine (timemgr_atforkparent) */

local void timemgr_atforkchild() noex {
        timemgr		*tmp = &timemgr_data ;
        if (tmp->fl.workready) {
            tmp->fl.running_siger = false ;
            tmp->fl.running_disper = false ;
	    if_constexpr (f_childthrs) {
                if (tmp->fl.thrs) {
                    tmp->fl.thrs = false ;
                    tmp->thrsbegin() ;
                }
	    } else {
                tmp->fl.thrs = false ;
                tmp->workdump() ;
	    }
        } /* end if (was "working") */
        tmp->capend() ;
} /* end subroutine (timemgr_atforkchild) */

local void timemgr_exit() noex {
	timemgr_data.fvoid = true ;
} /* end subroutine (timemgr_atforkparent) */

int uctimxnote::load (void *op,psem *psp,uctimx_f fp,int a) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    notf	= fp ;		/* notify function (C-linkage) */
	    objp	= op ;		/* object pointer (function argument) */
	    psemp	= psp ;		/* POSIX® Semaphore pointer */
	    notarg	= a ;		/* notification function argument */
	} /* end if (non-null) */
	return rs ;
} /* end method (uctimxnote::load) */

local void uctimxent_load(uctimxent *ep,con uctimxnote *nop) noex {
    	ep->val = {} ;
	ep->id = 0 ;
	ep->notf	= nop->notf ;
	ep->objp	= nop->objp ;
	ep->psemp	= nop->psemp ;
	ep->notarg	= nop->notarg ;
} /* end subroutine (uctimxeent_load) */

local int cmpuctimxent(con uctimxent *e1p,con uctimxent *e2p) noex {
    	return saturate<int>(e1p->val - e2p->val) ;
} /* end subroutine (cmpuctimxent) */

local int cmpqent(cvoid *v1p,cvoid *v2p) noex {
	con uctimxent	*e1p = resumelife<uctimxent>(v1p) ;
	con uctimxent	*e2p = resumelife<uctimxent>(v2p) ;
	int		rc = 0 ;
	if (e1p || e2p) ylikely {
	    rc = +1 ;
	    if (e1p) {
		rc = -1 ;
	        if (e2p) {
	            rc = cmpuctimxent(e1p,e2p) ;
	        }
	    }
	} /* end if */
	return rc ;
} /* end subroutine (cmpqent) */



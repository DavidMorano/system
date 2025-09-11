/* uctim SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++11 */

/* interface components for UNIX® library-3c */
/* virtual per-process timer management */
/* version %I% last-modified %G% */

#define	CF_CHILDTHRS	0		/* start threads in child process */

/* revision history:

	= 2014-04-04, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_timcreate
	uc_timdestroy
	uc_timset
	uc_timget
	uc_timover

	Description:
	This module creates per-process (virtual) time-of-day timers
	for callers.  This interface (these subroutines) are meant
	to mimic the POSIX® real-time per-process timer facility.
	Why was this necessary?  Becuase somt (stupid) operatoring
	systems which will not be named but have the initials --
	Apple Darwin -- do not have the POSIX® rea-time per-process
	timers.  Just a note: unlinke the POSIX® real-time per-process
	timers, this favility only uses a timer resolution of
	microseconds rather than nanoseconds (which the POSIX®
	interface uses).

	Synopsis:
	int uc_timcreate(itcontrol *itcp) noex
	int uc_timdestroy(int id) noex
	int uc_timset(int id,int,CITIMERVAL *ntvp,ITIMERVAL *otvp) noex
	int uc_timget(int id,ITIMERVAL *otvp) noex
	int uc_timover(int id) noex

	Arguments:
	itcp		itcontrol object pointer
	id		timer identification
	ntvp		new timer-value-pointer
	otvp		old timer-value-pointer

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/time.h>		/* <- interval timers are here */
#include	<pthread.h>		/* |PTHREAD_SCOPE_SYSTEM| */
#include	<ucontext.h>
#include	<csignal>
#include	<ctime>			/* i-timer types */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<queue>
#include	<usystem.h>
#include	<upt.h>
#include	<timewatch.hh>
#include	<ptm.h>
#include	<ptc.h>
#include	<pta.h>
#include	<vechand.h>		/* vector-handles */
#include	<vecsorthand.h>		/* vector-sorted-handles */
#include	<ciq.h>			/* container-interlocked-queue */
#include	<timespec.h>
#include	<itimerspec.h>
#include	<sigevent.h>
#include	<localmisc.h>

#include	"itcontrol.h"


/* local defines */

#ifndef	CF_CHILDTHRS
#define	CF_CHILDTHRS	0
#endif

#define	UCTIM_SCOPE	PTHREAD_SCOPE_SYSTEM

#define	TO_CAPTURE	60		/* timeout: capture wait for threads */
#define	TO_SIGWAIT	2		/* timeout: signal-process wait */
#define	TO_DISPRECV	5		/* timeout: dispatch-process wait */


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef int (*tworker_f)(void *) noex ;
}

typedef	UCTIM_FL	uctim_fl ;
typedef vecsorthand	prique ;
typedef itcontrol *	itcontrolp ;


/* external subroutines */


/* external variables */


/* local structures */

enum dispcmds {
	dispcmd_exit,
	dispcmd_timeout,
	dispcmd_handle,
	dispcmd_overlast
} ;

enum cmdsubs {
	cmdsub_create,
	cmdsub_destroy,
	cmdsub_set,
	cmdsub_get,
	cmdsub_over,
	cmdsub_overlast
} ;

namespace {
    struct uctimarg {
	itcontrol	*itcp ;
	CITIMERVAL	*ntcp ;
	ITIMERVAL	*otcp ;
	uctimarg(itcontrol *c) noex : itcp(c) { } ;
	uctimarg(ITIMERVAL *o,CITIMERVAL *n) noex : ntvp(n), otvp(o) { } ;
	int operator (cmdsubs,int) noex ;
    } ;
    struct uctim_flags {
	uint		timer:1 ;	/* UNIX®-RT timer created */
	uint		workready:1 ;
	uint		thrs:1 ;
	uint		wasblocked:1 ;
	uint		running_siger:1 ;
	uint		running_disper:1 ;
    } ;
    struct uctim {
	ptm		mx ;		/* data mutex */
	ptc		cv ;		/* condition variable */
	vechand		ents ;
	ciq		pass ;
	prique		*pqp ;
	sigset_t	savemask ;
	pthread_t	tid_siger ;
	pthread_t	tid_disper ;
	uctim_fl	fl ;
	volatile int	waiters ;	/* n-waiters for general capture */
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
	int init() noex ;
	int fini() noex ;
	int cmd_create(int,uctimarg *) noex ;
	int cmd_destroy(int,uctimarg *) noex ;
	int cmd_set(int,uctimarg *) noex ;
	int cmd_get(int,uctimarg *) noex ;
	int cmd_over(int,uctimarg *) noex ;
	int cmdsub(cmdsubs,int,uctimarg *) noex ;
	int capbegin(int = -1) noex ;
	int capend() noex ;
	int enterpri(itcontrol *) noex ;
	int timerset(time_t) noex ;
	int workready() noex ;
	int workbegin() noex ;
	int workend() noex ;
	int workfins() noex ;
	int workdump() noex ;
	int priqbegin() noex ;
	int priqend() noex ;
	int pridump() noex ;
	int sigbegin() noex ;
	int sigend() noex ;
	int timerbegin() noex ;
	int timerend() noex ;
	int thrsbegin() noex ;
	int thrsend() noex ;
	int sigerbegin() noex ;
	int sigerend() noex ;
	int sigerworker() noex ;
	int sigerwait() noex ;
	int sigerserve() noex ;
	int sigerdump() noex ;
	int dispbegin() noex ;
	int dispend() noex ;
	int dispworker() noex ;
	int disprecv() noex ;
	int disphandle() noex ;
	int dispjobdel(itcontrol *) noex ;
	destruct uctim() noex {
	    if (cint rs = fini() ; rs < 0) {
		ulogerror("uctim",rs,"dtor-fini") ;
	    }
	} ; /* end dtor (uctim) */
    } ; /* end struct (uctim) */
}


/* forward references */

static int	ourcmp(const TIMEOUT *,const TIMEOUT *) noex ;

extern "C" {
    static int	uctim_sigerworker(uctim *) noex ;
    static int	uctim_dispworker(uctim *) noex ;
    static void	uctim_atforkbefore() noex ;
    static void	uctim_atforkparent() noex ;
    static void	uctim_atforkchild() noex ;
    static void	uctim_exit() noex ;
}


/* local variables */

static uctim		uctim_data ;

constexpr bool		f_childthrs = CF_CHILDTHRS ;


/* exported variables */


/* exported subroutines */

int uc_timcreate(itcontrol *itcp) noex {
	uctimarg	ao(itcp) ;
	return ao(cmdsub_create,0) ;
}

int uc_timdestroy(int id) noex {
	uctimarg	ao ;
	return ao(cmdsub_destroy,id) ;
}

int uc_timset(int id,CITIMERVAL *ntvp,ITIMERVAL *otvp) noex {
	uctimarg	ao(otvp,ntvp) ;
	return ao(cmdsub_set,id) ;
}

int uc_timget(int id,ITIMERVAL *otvp) noex {
	uctimarg	ao(otvp) ;
	return ao(cmdsub_get,id) ;
}

int uc_timover(int id) noex {
	uctimarg	ao ;
	return ao(cmdsub_over,id) ;
}


/* local subroutines */

int uctimarg::operator () (cmdsubs cmd,int id) noex {
	return uctim_data.cmdsub(cmd,id,this) ;
}
/* end subroutine (uctimarg::operator) */

int uctim::cmdsub(cmdsubs cmd,int id,uctimarg *uap) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rv = 0 ;
	if (uap) {
	    rs = SR_INVALID ;
	    if (cmd >= 0) {
	        if ((rs = init()) >= 0) {
	            if ((rs = capbegin()) >= 0) {
	                if ((rs = workready()) >= 0) {
	                    switch (cmd) {
	                    case cmdsub_create:
	                        rs = cmd_create(id,uap) ;
	                        break ;
	                    case cmdsub_destroy:
	                        rs = cmd_destroy(id,uap) ;
	                        break ;
	                    case cmdsub_set:
	                        rs = cmd_set(id,uap) ;
	                        break ;
	                    case cmdsub_get:
	                        rs = cmd_get(id,uap) ;
	                        break ;
	                    case cmdsub_over:
	                        rs = cmd_over(id,uap) ;
	                        break ;
	                    default:
	                        rs = SR_INVALID ;
	                        break ;
	                    } /* end switch */
			    rv = rs ;
	                } /* end if (uctim_workready) */
	                rs1 = capend() ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (uctim-cap) */
	        } /* end if (uctim_init) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (uctim::cmdsub) */

int uctim::init() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (! fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    rs = SR_OK ;
	    if (! finit.testandset) {
	        if ((rs = mx.create) >= 0) {
	            if ((rs = cv.create) >= 0) {
	                void_f	b = uctim_atforkbefore ;
	                void_f	ap = uctim_atforkparent ;
	                void_f	ac = uctim_atforkchild ;
	                if ((rs = uc_atfork(b,ap,ac)) >= 0) {
			    void_f	e = uctim_exit ;
	                    if ((rs = uc_atexit(e)) >= 0) {
	                        finitdone = true ;
	                        f = true ;
	                    }
	                    if (rs < 0) {
	                        uc_atforkexpunge(b,ap,ac) ;
			    }
	                } /* end if (uc_atfork) */
	                if (rs < 0) {
	                    cv.destroy() ;
			}
	            } /* end if (ptc::create) */
	            if (rs < 0) {
	                mx.destroy() ;
		    }
	        } /* end if (ptm::create) */
	        if (rs < 0) {
	            finit = false ;
		}
	    } else if (! finitdone) {
	        timewatch	tw(to) ;
	        auto lamb = [this] () -> int {
	            int		rs = SR_OK ;
	            if (!finit) {
		        rs = SR_LOCKLOST ;
	            } else if (finitdone) {
		        rs = 1 ;
	            }
	            return rs ;
	        } ; /* end lambda */
	        rs = tw(lamb) ;			/* <- time-watching */
	    } /* end if (initialization) */
	} /* end if (not-voided) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (uctim::init) */

int uctim::fini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone && (! fvoid.testandset)) {
	    {
	        rs1 = workend() ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        void_f	b = uctim_atforkbefore ;
	        void_f	ap = uctim_atforkparent ;
	        void_f	ac = uctim_atforkchild ;
	        rs1 = uc_atforkexpunge(b,ap,ac) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = cv.destroy ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = mx.destroy ;
		if (rs >= 0) rs = rs1 ;
	    }
	    finit = false ;
	    finitdone = false ;
	} /* end if (was initialized) */
	return rs ;
}
/* end subroutine (uctim::fini) */

int uctim::cmd_create(int id,uctimarg *argp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (valp->metf) {
	    cint	esz = szof(itcontrol) ;
	    if (itcontrol *ep ; (rs = lm_mall(esz,&ep)) >= 0) {
		*ep = *argp->itcp ;
	        if ((rs = ents.add(ep)) >= 0) {
	            cint	ei = rs ;
	            *ep = *valp ;
	            {
	                ep->id = ei ;
	                rs = enterpri(ep) ;
	            }
	            if (rs < 0) {
	                ents.del(ei) ;
		    }
	        } /* end if (vechand_add) */
	        if (rs < 0) {
	            lm_free(ep) ;
		}
	    } /* end if (m-a) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uctim::cmd_create) */

int uctim::cmd_set(int id,uctimarg *argp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (valp->metf) {
	    cint	esz = szof(itcontrol) ;
	    if (itcontrol *ep ; (rs = lm_mall(esz,&ep)) >= 0) {
	        if ((rs = mx.lockbegin) >= 0) {
	            if ((rs = ents.add(ep)) >= 0) {
	                cint	ei = rs ;
	                *ep = *valp ;
	                {
	                    ep->id = ei ;
	                    rs = enterpri(ep) ;
	                }
	                if (rs < 0) {
	                    ents.del(ei) ;
			}
	            } /* end if (vechand_add) */
	            rs1 = mx.lockend ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (ptm) */
	        if (rs < 0) {
	            lm_free(ep) ;
		}
	    } /* end if (m-a) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uctim::cmd_set) */

int uctim::cmd_destroy(int id,uctimarg *argp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mx.lockbegin) >= 0) {
	    cint	id = valp->id ;
	    if (void *vp ; (rs = ents.get(id,&vp)) >= 0) {
	        itcontrol	*ep = (itcontrol *) vp ;
	        cint		ei = rs ;
	        if ((rs = ents.del(ei)) >= 0) {
	            cint	rsn = SR_NOTFOUND ;
		    bool	f_free = false ;
	            if ((rs = pqp->delhand(ep)) >= 0) {
			f_free = true ;
		    } else if (rs == rsn) {
	                ciq	*cqp = &pass ;
	                if ((rs = ciq_rement(cqp,ep)) >= 0) {
			    f_free = true ;
			} else if (rs == rsn) {
	                    rs = SR_OK ;
	                }
		    }
		    if ((rs >= 0) && f_free) {
	            	rs = lm_free(ep) ;
		    }
	        } /* end if (vechand_del) */
	    } /* end if (vechand_get) */
	    rs1 = mx.lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return rs ;
}
/* end subroutine (uctim::cmd_destroy) */

int uctim::cmd_over(int id,uctimarg *argp) noex {
    	int		rs = SR_OK ;
	(void) id ;
	(void) argp ;
	return rs ;;
}

int uctim::enterpri(itcontrol *ep) noex {
	int		rs ;
	int		pi = 0 ;
	if ((rs = pqp->count) > 0) {
	    if (itcontrol *tep ; (rs = pqp->get(0,&tep)) >= 0) {
	        if (ep->val < tep->val) {
	            if ((rs = pqp->add(ep)) >= 0) {
	                pi = rs ;
	                rs = timerset(ep->val) ;
	                if (rs < 0)
	                    pqp->del(pi) ;
	            }
	        } else {
	            rs = pqp->add(ep) ;
	            pi = rs ;
	        }
	    } /* end if (vecsorthand_get) */
	} else {
	    if ((rs = pqp->add(ep)) >= 0) {
	        pi = rs ;
	        rs = timerset(ep->val) ;
	        if (rs < 0) {
	            pqp->del(pi) ;
		}
	    } /* end if (vecsorthand_add) */
	}
	return (rs >= 0) ? pi : rs ;
}
/* end subroutine (uctim::enterpri) */

int uctim::timerset(time_t val) noex {
    	cnullptr	np{} ;
	int		rs ;
	if (TIMESPEC ts ; (rs = timespec_load(&ts,val,0)) >= 0) {
	    if (ITIMERSPEC it ; (rs = itimerspec_load(&it,&ts,np)) >= 0) {
	        cint	tf = TIMER_ABSTIME ;
	        rs = uc_timerset(timerid,tf,&it,nullptr) ;
	    }
	}
	return rs ;
}
/* end subroutine (uctim::timerset) */

int uctim::capbegin(int to) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mx.lockbegin(to)) >= 0) {
	    waiters += 1 ;
	    while ((rs >= 0) && fcapture) { /* busy */
	        rs = cv.wait(&mx,to) ;
	    } /* end while */
	    if (rs >= 0) {
	        fcapture = true ;
	    }
	    waiters -= 1 ;
	    rs1 = mx.lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return rs ;
}
/* end subroutine (uctim::capbegin) */

int uctim::capend() noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mx.lockbegin) >= 0) {
	    fcapture = false ;
	    if (waiters > 0) {
	        rs = cv.signal ;
	    }
	    rs1 = mx.lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return rs ;
}
/* end subroutine (uctim::capend) */

int uctim::workready() noex {
	int		rs = SR_OK ;
	if (! fl.workready) {
	    rs = workbegin() ;
	}
	if ((rs >= 0) && (! fl.thrs)) {
	    rs = thrsbegin() ;
	}
	return rs ;
}
/* end subroutine (uctim::workready) */

int uctim::workbegin() noex {
	int		rs = SR_OK ;
	if (! fl.workready) {
	    cint	vn = 0 ;
	    cint	vo = (vechandm.compact | vechandm.ordered) ;
	    if ((rs = ents.start(vn,vo)) >= 0) {
	        if ((rs = priqbegin()) >= 0) {
	            if ((rs = sigbegin()) >= 0) {
	                if ((rs = timerbegin()) >= 0) {
	                    if ((rs = ciq_start(&pass)) >= 0) {
	                        if ((rs = thrsbegin()) >= 0) {
	                            fl.workready = true ;
	                        }
	                        if (rs < 0) {
	                            ciq_finish(&pass) ;
	                        }
	                    }
	                    if (rs < 0) {
	                        timerend() ;
	                    }
	                } /* end if (uctim::timerbegin) */
	                if (rs < 0) {
	                    sigend() ;
			}
	            } /* end if (uctim::sigbegin) */
	            if (rs < 0) {
	                priqend() ;
	            }
	        } /* end if (uctim::pribegin) */
	        if (rs < 0) {
	            ents.finish() ;
	        }
	    } /* end if (vechand_start) */
	} /* end if (needed) */
	return rs ;
}
/* end subroutine (uctim::workbegin) */

int uctim::workend() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (fl.workready) {
	    {
	        rs1 = thrsend() ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ciq_finish(&pass) ;
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
	        rs1 = workfins() ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ents.finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    fl.workready = false ;
	} /* end if (work-ready) */
	return rs ;
}
/* end subroutine (uctim::workend) */

int uctim::workfins() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	void		*otp{} ;
	for (int i = 0 ; ents.get(i,&otp) >= 0 ; i += 1) {
	    if (otp) {
	        rs1 = lm_free(otp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (uctim_workfins) */

int uctim::workdump() noex {
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
                rs1 = workfins() ;
                if (rs >= 0) rs = rs1 ;
	    }
        } /* end if (work-ready) */
        return rs ;
}
/* end subroutine (uctim::workdump) */

int uctim::priqbegin() noex {
	cint		osz = szof(vecsorthand) ;
	int		rs ;
	if (void *p ; (rs = lm_mall(osz,&p)) >= 0) {
	    prique	*pqp = (prique *) p ;
	    rs = pqp->start(ourcmp,1) ;
	    if (rs < 0) {
	        lm_free(pqp) ;
	        pqp = nullptr ;
	    }
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (uctim::priqbegin) */

int uctim::priqend() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (pqp) {
	    {
	        rs1 = pqp->finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = lm_free(pqp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    pqp = nullptr ;
	}
	return rs ;
}
/* end subroutine (uctim::priqend) */

int uctim::pridump() noex {
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
}
/* end subroutine (uctim::pridump) */

int uctim::sigbegin() noex {
	sigset_t	ss ;
	sigset_t	oss ;
	cint		scmd = SIG_BLOCK ;
	cint		sig = SIGALARM ;
	int		rs ;
	uc_sigsetempty(&ss) ;
	uc_sigsetadd(&ss,sig) ;
	if ((rs = u_sigmask(scmd,&ss,&oss)) >= 0) {
	    if ((rs = uc_sigsetismem(&ss,sig)) > 0) {
	        fl.wasblocked = true ;
	    }
	}
	return rs ;
}
/* end subroutine (uctim::sigbegin) */

int uctim::sigend() noex {
	int		rs = SR_OK ;
	if (! fl.wasblocked) {
	    sigset_t	ss ;
	    cint	scmd = SIG_UNBLOCK ;
	    cint	sig = SIGALARM ;
	    uc_sigsetempty(&ss) ;
	    uc_sigsetadd(&ss,sig) ;
	    rs = u_sigmask(scmd,&ss,nullptr) ;
	}
	return rs ;
}
/* end subroutine (uctim::sigend) */

int uctim::timerbegin() noex {
	cint		st = SIGEV_SIGNAL ;
	cint		sig = SIGALARM ;
	cint		val = 0 ; /* we do not (really) care about this */
	int		rs ;
	if (SIGEVENT se ; (rs = sigevent_load(&se,st,sig,val)) >= 0) {
	    const clockid_t	cid = CLOCK_REALTIME ;
	    timer_t		tid ;
	    if ((rs = uc_timercreate(cid,&se,&tid)) >= 0) {
	        timerid = tid ;
	        fl.timer = true ;
	    }
	}
	return rs ;
}
/* end subroutine (uctim::timerbegin) */

int uctim::timerend() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = uc_timerdestroy(timerid) ;
	    if (rs >= 0) rs = rs1 ;
	}
	timerid = 0 ;
	fl.timer = false ;
	return rs ;
}
/* end subroutine (uctim::timerend) */

int uctim::thrsbegin() noex {
	int		rs = SR_OK ;
	if ((! fl.thrs) && (! freqexit)) {
	    if ((rs = sigerbegin()) >= 0) {
	        if ((rs = dispbegin()) >= 0) {
	            fl.thrs = true ;
	        }
	        if (rs < 0) {
	            sigerend() ;
	        }
	    }
	} /* end if (needed) */
	return rs ;
}
/* end subroutine (uctim::thrsbegin) */

int uctim::thrsend() noex {
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
}
/* end subroutine (uctim::thrsend) */

int uctim::sigerbegin() noex {
	int		rs ;
	int		rs1 ;
	int		f = false ;
	if (pta ta ; (rs = ta.create) >= 0) {
	    cint	scope = UCTIM_SCOPE ;
	    if ((rs = ta.setscope(scope)) >= 0) {
	        tworker_f	wt = tworker_f(uctim_sigerworker) ;
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
}
/* end subroutine (uctim_sigerbegin) */

int uctim::sigerend() noex {
	int		rs = SR_OK ;
	if (fl.running_siger) {
	    pthread_t	tid = tid_siger ;
	    cint	sig = SIGALARM ;
	    if ((rs = uptkill(tid,sig)) >= 0) {
	        int	trs ;
	        fl.running_siger = false ;
	        if ((rs = uptjoin(tid,&trs)) >= 0) {
	            rs = trs ;
	        } else if (rs == SR_SRCH) {
	            rs = SR_OK ;
	        }
	    } /* end if (uptkill) */
	} /* end if (running) */
	return rs ;
}
/* end subroutine (uctim_sigerend) */

/* this is an independent thread of execution */
int uctim::sigerworker() noex {
	int		rs ;
	while ((rs = sigerwait()) > 0) {
	    if (freqexit) break ;
	    switch (rs) {
	    case 1:
	        rs = sigerserve() ;
	        break ;
	    }
	    if (rs < 0) break ;
	} /* end while */
	fexitsiger = true ;
	return rs ;
}
/* end subroutine (uctim::sigerworker) */

int uctim::sigerwait() noex {
	sigset_t	ss ;
	cint		sig = SIGALARM ;
	cint		to = TO_SIGWAIT ;
	int		rs ;
	int		cmd = 0 ;
	bool		f_timedout = false ;
	uc_sigsetempty(&ss) ;
	uc_sigsetadd(&ss,sig) ;
	uc_sigsetadd(&ss,SIGALRM) ;
	if (TIMESPEC ts ; (rs = timespec_load(&ts,to,0)) >= 0) {
	    siginfo_t	si{} ;
	    bool	f_exit = false ;
	    repeat {
	        if ((rs = uc_sigwaitinfoto(&ss,&si,&ts)) < 0) {
	            switch (rs) {
	            case SR_INTR:
	                break ;
	            case SR_AGAIN:
	                f_timedout = true ;
	                rs = SR_OK ; /* will cause exit from loop */
	                break ;
	            default:
	                f_exit = true ;
	                break ;
	            } /* end switch */
	        } /* end if (error) */
	    } until ((rs >= 0) || f_exit) ;
	} /* end if (timespec) */
	if (rs >= 0) {
	    if (! freqexit) {
	        if (f_timedout) {
	            cmd = dispcmd_handle ;
	        } else if (sig == si.si_signo) {
	            cmd = dispcmd_timeout ;
	        }
	    } /* end if (not exiting) */
	} /* end if (ok) */
	return (rs >= 0) ? cmd : rs ;
}
/* end subroutine (uctim::sigerwait) */

int uctim::sigerserve() noex {
	cint		to = TO_CAPTURE ;
	int		rs ;
	int		rs1 ;
	if ((rs = capbegin(to)) >= 0) {
	    custime	dt = time(nullptr) ;
	    while ((rs = pqp->count) > 0) {
	        if (itcontrol *tep ; (rs = pqp->get(0,&tep)) >= 0) {
	            cint	ei = rs ;
	            if (tep->val > dt) break ;
	            if ((rs = pqp->del(ei)) >= 0) {
	                if ((rs = ciq_ins(&pass,tep)) >= 0) {
	                    fcmd = true ;
	                    rs = cv.signal ;
	                }
	            }
	        }
	        if (rs < 0) break ;
	    } /* end while */
	    rs1 = capend() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (capture) */
	return rs ;
}
/* end subroutine (uctim::sigerserve) */

int uctim::sigerdump() noex {
        ciq             *cqp = &pass ;
        int             rs = SR_OK ;
        int             rs1 ;
        void            *tep ;
        while ((rs1 = ciq_rem(cqp,&tep)) >= 0) ; /* loop */
        if ((rs >= 0) && (rs1 != SR_NOTFOUND)) rs = rs1 ;
        return rs ;
}
/* end subroutine (uctim::sigerdump) */

int uctim::dispbegin() noex {
	int		rs ;
	int		rs1 ;
	int		f = false ;
	if (pta ta ; (rs = ta.create) >= 0) {
	    cint	scope = UCTIM_SCOPE ;
	    if ((rs = ta.setscope(scope)) >= 0) {
	        tworker_f	wt = tworker_f(uctim_dispworker) ;
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
}
/* end subroutine (uctim::dispbegin) */

int uctim::dispend() noex {
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
}
/* end subroutine (uctim::dispend) */

/* it always takes a good bit of code to make this part look easy! */
int uctim::dispworker() noex {
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
}
/* end subroutine (uctim::dispworker) */

int uctim::disprecv() noex {
	int		rs ;
	int		rs1 ;
	int		to = TO_DISPRECV ;
	int		cmd = dispcmd_exit ;
	if ((rs = mx.lockbegin) >= 0) {
	    waiters += 1 ;
	    while ((rs >= 0) && (! fcmd)) {
	        rs = cv.wait(&mx,to) ;
	    } /* end while */
	    if (rs >= 0) {
	        fcmd = false ;
	        if (! freqexit) cmd = dispcmd_handle ;
	        if (waiters > 1) {
	            rs = cv.signal ;
	        }
	    } else if (rs == SR_TIMEDOUT) {
	        if (! freqexit) cmd = dispcmd_timeout ;
	        rs = SR_OK ;
	    } else {
	        cmd = dispcmd_exit ;
	    }
	    waiters -= 1 ;
	    rs1 = mx.lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (mutex-section) */
	return (rs >= 0) ? cmd : rs ;
}
/* end subroutine (uctim::disprecv) */

int uctim::disphandle() noex {
	itcontrol	*tep ;
	int		rs = SR_OK ;
	int		rs1 ;
	while ((rs1 = ciq_rem(&pass,&tep)) >= 0) {
	    if ((rs = dispjobdel(tep)) > 0) {
	        timeout_f	met = (timeout_f) tep->metf ;
	        rs = (*met)(tep->objp,tep->tag,tep->arg) ;
	        lm_free(tep) ;
	    } /* end if (still had job) */
	    if (rs < 0) break ;
	} /* end while */
	if ((rs >= 0) && (rs1 != SR_EMPTY)) rs = rs1 ;
	return rs ;
}
/* end subroutine (uctim::disphandle) */

int uctim::dispjobdel(itcontrol *tep) noex {
        cint       	to = TO_CAPTURE ;
	int		rs ;
	int		rs1 ;
	int		f = false ;
        if ((rs = capbegin(to)) >= 0) {
	    if ((rs = ents.delhand(tep)) >= 0) {
		f = true ;
	    } else if (rs == SR_NOTFOUND) {
		rs = SR_OK ;
	    }
	    rs1 = capend() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (uctim-cap) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (uctim::dispjobdel) */

static int uctim_sigerworker(uctim *uip) noex {
	return uip->sigerworker() ;
}

static int uctim_dispworker(uctim *uip) noex {
	return uip->dispworker() ;
}

static void uctim_atforkbefore() noex {
	uctim_data.mx.lockbegin() ;
}
/* end subroutine (uctim_atforkbefore) */

static void uctim_atforkparent() noex {
	uctim_data.mx.lockend() ;
}
/* end subroutine (uctim_atforkparent) */

static void uctim_atforkchild() noex {
        uctim       *uip = &uctim_data ;
        if (uip->fl.workready) {
            uip->fl.running_siger = false ;
            uip->fl.running_disper = false ;
	    if_constexpr (f_childthrs) {
                if (uip->fl.thrs) {
                    uip->fl.thrs = false ;
                    uip->thrsbegin() ;
                }
	    } else {
                uip->fl.thrs = false ;
                uip->workdump() ;
	    }
        } /* end if (was "working") */
        uip->capend() ;
}
/* end subroutine (uctim_atforkchild) */

static void uctim_exit() noex {
	uctim_data.fvoid = true ;
}
/* end subroutine (uctim_atforkparent) */

static int ourcmp(const TIMEOUT *e1p,const TIMEOUT *e2p) noex {
	int		rc = 0 ;
	if (e1p || e2p) {
	    rc = +1 ;
	    if (e1p) {
		rc = -1 ;
	        if (e2p) {
	            rc = (e1p->val - e2p->val) ;
	        }
	    }
	}
	return rc ;
}
/* end subroutine (ourcmp) */



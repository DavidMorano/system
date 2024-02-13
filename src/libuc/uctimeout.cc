/* uctimeout SUPPORT */
/* lang=C++11 */

/* UNIX® time-out management */
/* version %I% last-modified %G% */

#define	CF_CHILDTHRS	0		/* start threads in child process */

/* revision history:

	= 2014-04-04, David A­D­ Morano
	Originally written for Rightcore Network Services.

	= 2018-10-12, David A.D. Morano
	Added some small error resiliency to |uctimeout_enterpri()|.

*/

/* Copyright © 2014,2018 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_timeout

	Description:
	This subroutine manages time-outs (registered call-backs).
	This subroutine acts like a process-wide system call, and
	can be used by all threads within the process for creating
	time-out call-backs. Of course this is thread-safe.

	Synopsis:
	int uc_timeout(int cmd,TIMEOUT *val) noex

	Arguments:
	cmd		command:
			    timeoutcmd_set,
			    timeoutcmd_cancel
	val		pointer to 'timeout' value

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<ucontext.h>
#include	<csignal>
#include	<cstring>
#include	<ctime>
#include	<queue>
#include	<usystem.h>
#include	<usupport.h>
#include	<timewatch.hh>
#include	<ptm.h>
#include	<ptc.h>
#include	<pta.h>
#include	<upt.h>
#include	<vechand.h>		/* vector-handles */
#include	<vecsorthand.h>		/* vector-sorted-handles */
#include	<ciq.h>			/* container-interlocked-queue */
#include	<timespec.h>
#include	<itimerspec.h>
#include	<sigevent.h>
#include	<localmisc.h>

#include	"timeout.h"


/* local defines */

#ifndef	CF_CHILDTHRS
#define	CF_CHILDTHRS	0
#endif

#define	UCTIMEOUT	struct uctimeout
#define	UCTIMEOUT_FL	struct uctimeout_flags
#define	UCTIMEOUT_SCOPE	PTHREAD_SCOPE_SYSTEM

#define	TO_CAPTURE	60		/* capture wait for threads */
#define	TO_SIGWAIT	2		/* signal-process wait */
#define	TO_DISPRECV	5		/* dispatch-process wait */


#define	NDF		"uctimeout.deb"


/* local typedefs */

extern "C" {
    typedef int (*tworker)(void *) noex ;
}

typedef vecsorthand	prique ;


/* external subroutines */

extern "C" int		uc_timeout(int,TIMEOUT *) noex ;

extern "C" cchar	*strsigabbr(int) ;


/* local structures */

namespace {
    struct uctimeout_flags {
	uint		timer:1 ;	/* UNIX®-RT timer created */
	uint		workready:1 ;
	uint		thrs:1 ;
	uint		wasblocked:1 ;
	uint		running_siger:1 ;
	uint		running_disper:1 ;
    } ;
    struct uctimeout {
	ptm		mx ;		/* data mutex */
	ptc		cv ;		/* condition variable */
	vechand		ents ;
	ciq		pass ;
	UCTIMEOUT_FL	fl ;
	vecsorthand	*pqp ;
	sigset_t	savemask ;
	pid_t		pid ;
	pthread_t	tid_siger ;
	pthread_t	tid_disper ;
	timer_t		timerid ;
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
	int cmdset(TIMEOUT *) noex ;
	int cmdcancel(TIMEOUT *) noex ;
	int cmdtimeout(int,TIMEOUT *) noex ;
	int capbegin(int = -1) noex ;
	int capend() noex ;
	int enterpri(TIMEOUT *) noex ;
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
	int dispjobdel(TIMEOUT *) noex ;
	~uctimeout() noex {
	    cint	rs = fini() ;
	    if (rs < 0) {
		ulogerror("uctimeout",rs,"dtor-fini") ;
	    }
	} ; /* end dtor (uctimeout) */
    } ; /* end struct (uctimeout) */
}

enum dispcmds {
	dispcmd_exit,
	dispcmd_timeout,
	dispcmd_handle,
	dispcmd_overlast
} ;


/* forward references */

extern "C" {
    static int uctimeout_sigerworker(uctimeout *) noex ;
    static int uctimeout_dispworker(uctimeout *) noex ;
    static void	uctimeout_atforkbefore() noex ;
    static void	uctimeout_atforkparent() noex ;
    static void	uctimeout_atforkchild() noex ;
    static void	uctimeout_exit() noex ;
    static int	ourcmp(cvoid *,cvoid *) noex ;
}

consteval int uctimeout_voents() noex {
	int	vo = 0 ;
	vo |= VECHAND_OSTATIONARY ;
	vo |= VECHAND_OREUSE ;
	vo |= VECHAND_OCOMPACT ;
	vo |= VECHAND_OSWAP ;
	vo |= VECHAND_OCONSERVE ;
	return vo ;
}


/* local variables */

static uctimeout	uctimeout_data ;

constexpr int		voents = uctimeout_voents() ;

constexpr bool		f_childthrs = CF_CHILDTHRS ;


/* exported subroutines */

int uc_timeout(int cmd,TIMEOUT *valp) noex {
	return uctimeout_data.cmdtimeout(cmd,valp) ;
}


/* local subroutines */

int uctimeout::cmdtimeout(int cmd,TIMEOUT *valp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		id = 0 ;
	if (valp) {
	    rs = SR_INVALID ;
	    if (cmd >= 0) {
	        if ((rs = init()) >= 0) {
	            if ((rs = capbegin()) >= 0) {
	                if ((rs = workready()) >= 0) {
	                    switch (cmd) {
	                    case timeoutcmd_set:
	                        rs = cmdset(valp) ;
	                        id = rs ;
	                        break ;
	                    case timeoutcmd_cancel:
	                        rs = cmdcancel(valp) ;
	                        id = rs ;
	                        break ;
	                    default:
	                        rs = SR_INVALID ;
	                        break ;
	                    } /* end switch */
	                } /* end if (uctimeout_workready) */
	                rs1 = capend() ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (uctimeout-cap) */
	        } /* end if (uctimeout_init) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? id : rs ;
}
/* end subroutine (uctimeout::cmdtimeout) */

int uctimeout::init() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (! fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    rs = SR_OK ;
	    if (! finit.testandset) {
	        if ((rs = mx.create) >= 0) {
	            if ((rs = cv.create) >= 0) {
	                void_f	b = uctimeout_atforkbefore ;
	                void_f	ap = uctimeout_atforkparent ;
	                void_f	ac = uctimeout_atforkchild ;
	                if ((rs = uc_atfork(b,ap,ac)) >= 0) {
	                    if ((rs = uc_atexit(uctimeout_exit)) >= 0) {
	                        finitdone = true ;
	                        pid = getpid() ;
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
	    } else if (!finitdone) {
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
/* end subroutine (uctimeout::init) */

int uctimeout::fini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone) {
	    fvoid = true ;
	    {
	        rs1 = workend() ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        void_f	b = uctimeout_atforkbefore ;
	        void_f	ap = uctimeout_atforkparent ;
	        void_f	ac = uctimeout_atforkchild ;
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
/* end subroutine (uctimeout::fini) */

int uctimeout::cmdset(TIMEOUT *valp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (valp->metp) {
	    TIMEOUT	*ep ;
	    cint	esize = sizeof(TIMEOUT) ;
	    if ((rs = uc_libmalloc(esize,&ep)) >= 0) {
	        if ((rs = mx.lockbegin) >= 0) {
	            vechand	*elp = &ents ;
	            if ((rs = vechand_add(elp,ep)) >= 0) {
	                cint	ei = rs ;
	                *ep = *valp ;
	                {
	                    ep->id = ei ;
	                    rs = enterpri(ep) ;
	                }
	                if (rs < 0)
	                    vechand_del(elp,ei) ;
	            } /* end if (vechand_add) */
	            rs1 = mx.lockend ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (ptm) */
	        if (rs < 0) {
	            uc_libfree(ep) ;
		}
	    } /* end if (m-a) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uctimeout::cmdset) */

int uctimeout::cmdcancel(TIMEOUT *valp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mx.lockbegin) >= 0) {
	    vechand	*elp = &ents ;
	    cint	id = valp->id ;
	    void	*vp ;
	    if ((rs = vechand_get(elp,id,&vp)) >= 0) {
	        TIMEOUT	*ep = (TIMEOUT *) vp ;
	        cint	ei = rs ;
	        if ((rs = vechand_del(elp,ei)) >= 0) {
	            cint	nrs = SR_NOTFOUND ;
		    bool	f_free = false ;
	            if ((rs = vecsorthand_delhand(pqp,ep)) >= 0) {
			f_free = true ;
		    } else if (rs == nrs) {
	                ciq	*cqp = &pass ;
	                if ((rs = ciq_rement(cqp,ep)) >= 0) {
			    f_free = true ;
			} else if (rs == nrs) {
	                    rs = SR_OK ;
	                }
		    }
		    if ((rs >= 0) && f_free) {
	            	rs = uc_libfree(ep) ;
		    }
	        } /* end if (vechand_del) */
	    } /* end if (vechand_get) */
	    rs1 = mx.lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return rs ;
}
/* end subroutine (uctimeout_cmdcancel) */

int uctimeout::enterpri(TIMEOUT *ep) noex {
	int		rs ;
	int		pi = 0 ;
	if ((rs = vecsorthand_count(pqp)) > 0) {
	    TIMEOUT	*tep ;
	    if ((rs = vecsorthand_get(pqp,0,&tep)) >= 0) {
	        if (ep->val < tep->val) {
	            if ((rs = vecsorthand_add(pqp,ep)) >= 0) {
	                pi = rs ;
	                rs = timerset(ep->val) ;
	                if (rs < 0)
	                    vecsorthand_del(pqp,pi) ;
	            }
	        } else {
	            rs = vecsorthand_add(pqp,ep) ;
	            pi = rs ;
	        }
	    } /* end if (vecsorthand_get) */
	} else {
	    if ((rs = vecsorthand_add(pqp,ep)) >= 0) {
	        pi = rs ;
	        rs = timerset(ep->val) ;
	        if (rs < 0)
	            vecsorthand_del(pqp,pi) ;
	    } /* end if (vecsorthand_add) */
	}
	return (rs >= 0) ? pi : rs ;
}
/* end subroutine (uctimeout::enterpri) */

int uctimeout::timerset(time_t val) noex {
	TIMESPEC	ts ;
	int		rs ;
	if ((rs = timespec_load(&ts,val,0)) >= 0) {
	    ITIMERSPEC	it ;
	    if ((rs = itimerspec_load(&it,&ts,nullptr)) >= 0) {
	        cint		tf = TIMER_ABSTIME ;
	        rs = uc_timerset(timerid,tf,&it,nullptr) ;
	    }
	}
	return rs ;
}
/* end subroutine (uctimeout::timerset) */

int uctimeout::capbegin(int to) noex {
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
/* end subroutine (uctimeout::capbegin) */

int uctimeout::capend() noex {
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
/* end subroutine (uctimeout::capend) */

int uctimeout::workready() noex {
	int		rs = SR_OK ;
	if (! fl.workready) {
	    rs = workbegin() ;
	}
	if ((rs >= 0) && (! fl.thrs)) {
	    rs = thrsbegin() ;
	}
	return rs ;
}
/* end subroutine (uctimeout::workready) */

int uctimeout::workbegin() noex {
	int		rs = SR_OK ;
	if (! fl.workready) {
	    static constexpr int	vo = voents ;
	    if ((rs = vechand_start(&ents,0,vo)) >= 0) {
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
	                } /* end if (uctimeout::timerbegin) */
	                if (rs < 0) {
	                    sigend() ;
			}
	            } /* end if (uctimeout::sigbegin) */
	            if (rs < 0) {
	                priqend() ;
	            }
	        } /* end if (uctimeout::pribegin) */
	        if (rs < 0) {
	            vechand_finish(&ents) ;
	        }
	    } /* end if (vechand_start) */
	} /* end if (needed) */
	return rs ;
}
/* end subroutine (uctimeout::workbegin) */

int uctimeout::workend() noex {
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
	        rs1 = vechand_finish(&ents) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    fl.workready = false ;
	} /* end if (work-ready) */
	return rs ;
}
/* end subroutine (uctimeout::workend) */

int uctimeout::workfins() noex {
	vechand		*elp = &ents ;
	int		rs = SR_OK ;
	int		rs1 ;
	void		*top ;
	for (int i = 0 ; vechand_get(elp,i,&top) >= 0 ; i += 1) {
	    if (top) {
	        rs1 = uc_libfree(top) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (uctimeout_workfins) */

int uctimeout::workdump() noex {
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
/* end subroutine (uctimeout::workdump) */

int uctimeout::priqbegin() noex {
	cint		osize = sizeof(vecsorthand) ;
	int		rs ;
	void		*p ;
	if ((rs = uc_libmalloc(osize,&p)) >= 0) {
	    prique	*pqp = (prique *) p ;
	    rs = vecsorthand_start(pqp,1,ourcmp) ;
	    if (rs < 0) {
	        uc_libfree(pqp) ;
	        pqp = nullptr ;
	    }
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (uctimeout::priqbegin) */

int uctimeout::priqend() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (pqp) {
	    rs1 = vecsorthand_finish(pqp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (pqp) {
	    rs1 = uc_libfree(pqp) ;
	    if (rs >= 0) rs = rs1 ;
	    pqp = nullptr ;
	}
	return rs ;
}
/* end subroutine (uctimeout::priqend) */

int uctimeout::pridump() noex {
        int             rs = SR_OK ;
        int             rs1 ;
        void            *tep ;
        for (int i = 0 ; (rs1 = vecsorthand_get(pqp,i,&tep)) >= 0 ; i += 1) {
            if (tep) {
                rs1 = vecsorthand_del(pqp,i--) ;
                if (rs >= 0) rs = rs1 ;
            }
        } /* end for */
        if ((rs >= 0) && (rs1 != SR_NOTFOUND)) rs = rs1 ;
        return rs ;
}
/* end subroutine (uctimeout::pridump) */

int uctimeout::sigbegin() noex {
	sigset_t	ss, oss ;
	cint		scmd = SIG_BLOCK ;
	cint		sig = SIGTIMEOUT ;
	int		rs ;
	uc_sigsetempty(&ss) ;
	uc_sigsetadd(&ss,sig) ;
	if ((rs = pt_sigmask(scmd,&ss,&oss)) >= 0) {
	    if ((rs = uc_sigsetismem(&ss,sig)) > 0) {
	        fl.wasblocked = true ;
	    }
	}
	return rs ;
}
/* end subroutine (uctimeout::sigbegin) */

int uctimeout::sigend() noex {
	int		rs = SR_OK ;
	if (! fl.wasblocked) {
	    sigset_t	ss ;
	    cint	scmd = SIG_UNBLOCK ;
	    cint	sig = SIGTIMEOUT ;
	    uc_sigsetempty(&ss) ;
	    uc_sigsetadd(&ss,sig) ;
	    rs = pt_sigmask(scmd,&ss,nullptr) ;
	}
	return rs ;
}
/* end subroutine (uctimeout::sigend) */

int uctimeout::timerbegin() noex {
	SIGEVENT	se ;
	cint		st = SIGEV_SIGNAL ;
	cint		sig = SIGTIMEOUT ;
	cint		val = 0 ; /* we do not (really) care about this */
	int		rs ;
	if ((rs = sigevent_load(&se,st,sig,val)) >= 0) {
	    const clockid_t	cid = CLOCK_REALTIME ;
	    timer_t		tid ;
	    if ((rs = uc_timercreate(cid,&se,&tid)) >= 0) {
	        timerid = tid ;
	        fl.timer = true ;
	    }
	}
	return rs ;
}
/* end subroutine (uctimeout::timerbegin) */

int uctimeout::timerend() noex {
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
/* end subroutine (uctimeout::timerend) */

int uctimeout::thrsbegin() noex {
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
/* end subroutine (uctimeout::thrsbegin) */

int uctimeout::thrsend() noex {
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
/* end subroutine (uctimeout::thrsend) */

int uctimeout::sigerbegin() noex {
	pta		ta ;
	int		rs ;
	int		rs1 ;
	int		f = false ;
	if ((rs = pta_create(&ta)) >= 0) {
	    cint	scope = UCTIMEOUT_SCOPE ;
	    if ((rs = pta_setscope(&ta,scope)) >= 0) {
	        pthread_t	tid ;
	        tworker		wt = (tworker) uctimeout_sigerworker ;
	        if ((rs = uptcreate(&tid,&ta,wt,this)) >= 0) {
	            fl.running_siger = true ;
	            tid_siger = tid ;
	            f = true ;
	        } /* end if (pthread-create) */
	    } /* end if (pta-setscope) */
	    rs1 = pta_destroy(&ta) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (pta) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (uctimeout_sigerbegin) */

int uctimeout::sigerend() noex {
	int		rs = SR_OK ;
	if (fl.running_siger) {
	    pthread_t	tid = tid_siger ;
	    cint	sig = SIGTIMEOUT ;
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
/* end subroutine (uctimeout_sigerend) */

/* this is an independent thread of execution */
int uctimeout::sigerworker() noex {
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
/* end subroutine (uctimeout::sigerworker) */

int uctimeout::sigerwait() noex {
	TIMESPEC	ts ;
	sigset_t	ss ;
	siginfo_t	si ;
	cint		sig = SIGTIMEOUT ;
	cint		to = TO_SIGWAIT ;
	int		rs ;
	int		cmd = 0 ;
	bool		f_exit = false ;
	bool		f_timedout = false ;
	uc_sigsetempty(&ss) ;
	uc_sigsetadd(&ss,sig) ;
	uc_sigsetadd(&ss,SIGALRM) ;
	timespec_load(&ts,to,0) ;
	repeat {
	    rs = uc_sigwaitinfoto(&ss,&si,&ts) ;
	    if (rs < 0) {
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
	if (rs >= 0) {
	    if (! freqexit) {
	        if (f_timedout) {
	            cmd = 2 ;
	        } else if (sig == si.si_signo) {
	            cmd = 1 ;
	        }
	    } /* end if (not exiting) */
	} /* end if (ok) */
	return (rs >= 0) ? cmd : rs ;
}
/* end subroutine (uctimeout::sigerwait) */

int uctimeout::sigerserve() noex {
	cint		to = TO_CAPTURE ;
	int		rs ;
	int		rs1 ;
	if ((rs = capbegin(to)) >= 0) {
	    const time_t	dt = time(nullptr) ;
	    while ((rs = vecsorthand_count(pqp)) > 0) {
	        TIMEOUT		*tep ;
	        if ((rs = vecsorthand_get(pqp,0,&tep)) >= 0) {
	            cint	ei = rs ;
	            if (tep->val > dt) break ;
	            if ((rs = vecsorthand_del(pqp,ei)) >= 0) {
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
/* end subroutine (uctimeout::sigerserve) */

int uctimeout::sigerdump() noex {
        ciq             *cqp = &pass ;
        int             rs = SR_OK ;
        int             rs1 ;
        void            *tep ;
        while ((rs1 = ciq_rem(cqp,&tep)) >= 0) ; /* loop */
        if ((rs >= 0) && (rs1 != SR_NOTFOUND)) rs = rs1 ;
        return rs ;
}
/* end subroutine (uctimeout::sigerdump) */

int uctimeout::dispbegin() noex {
	pta		ta ;
	int		rs ;
	int		rs1 ;
	int		f = false ;
	if ((rs = pta_create(&ta)) >= 0) {
	    cint	scope = UCTIMEOUT_SCOPE ;
	    if ((rs = pta_setscope(&ta,scope)) >= 0) {
	        pthread_t	tid ;
	        tworker		wt = (tworker) uctimeout_dispworker ;
	        if ((rs = uptcreate(&tid,&ta,wt,this)) >= 0) {
	            fl.running_disper = true ;
	            tid_disper = tid ;
	            f = true ;
	        } /* end if (uptcreate) */
	    } /* end if (pta-setscope) */
	    rs1 = pta_destroy(&ta) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (pta) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (uctimeout::dispbegin) */

int uctimeout::dispend() noex {
	int		rs = SR_OK ;
	if (fl.running_disper) {
	    pthread_t	tid = tid_disper ;
	    int		trs ;
	    fl.running_disper = false ;
	    if ((rs = uptjoin(tid,&trs)) >= 0) {
	        rs = trs ;
	    } else if (rs == SR_SRCH) {
	        rs = SR_OK ;
	    }
	} /* end if (running) */
	return rs ;
}
/* end subroutine (uctimeout::dispend) */

/* it always takes a good bit of code to make this part look easy! */
int uctimeout::dispworker() noex {
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
/* end subroutine (uctimeout::dispworker) */

int uctimeout::disprecv() noex {
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
/* end subroutine (uctimeout::disprecv) */

int uctimeout::disphandle() noex {
	TIMEOUT		*tep ;
	int		rs = SR_OK ;
	int		rs1 ;
	while ((rs1 = ciq_rem(&pass,&tep)) >= 0) {
	    if ((rs = dispjobdel(tep)) > 0) {
	        timeout_met	met = (timeout_met) tep->metp ;
	        rs = (*met)(tep->objp,tep->tag,tep->arg) ;
	        uc_libfree(tep) ;
	    } /* end if (still had job) */
	    if (rs < 0) break ;
	} /* end while */
	if ((rs >= 0) && (rs1 != SR_EMPTY)) rs = rs1 ;
	return rs ;
}
/* end subroutine (uctimeout::disphandle) */

int uctimeout::dispjobdel(TIMEOUT *tep) noex {
        cint       	to = TO_CAPTURE ;
	int		rs ;
	int		rs1 ;
	int		f = false ;
        if ((rs = capbegin(to)) >= 0) {
	    if ((rs = vechand_delhand(&ents,tep)) >= 0) {
		f = true ;
	    } else if (rs == SR_NOTFOUND) {
		rs = SR_OK ;
	    }
	    rs1 = capend() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (uctimeout-cap) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (uctimeout::dispjobdel) */

static int uctimeout_sigerworker(uctimeout *uip) noex {
	return uip->sigerworker() ;
}

static int uctimeout_dispworker(uctimeout *uip) noex {
	return uip->dispworker() ;
}

static void uctimeout_atforkbefore() noex {
	uctimeout_data.mx.lockbegin() ;
}
/* end subroutine (uctimeout_atforkbefore) */

static void uctimeout_atforkparent() noex {
	uctimeout_data.mx.lockend() ;
}
/* end subroutine (uctimeout_atforkparent) */

static void uctimeout_atforkchild() noex {
        uctimeout       *uip = &uctimeout_data ;
        uip->pid = getpid() ;
        if (uip->fl.workready) {
            uip->fl.running_siger = false ;
            uip->fl.running_disper = false ;
	    if constexpr (f_childthrs) {
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
/* end subroutine (uctimeout_atforkchild) */

static void uctimeout_exit() noex {
	uctimeout_data.fvoid = true ;
}
/* end subroutine (uctimeout_atforkparent) */

static int ourcmp(cvoid *a1p,cvoid *a2p) noex {
	TIMEOUT		**e1pp = (TIMEOUT **) a1p ;
	TIMEOUT		**e2pp = (TIMEOUT **) a2p ;
	int		rc = 0 ;
	if (*e1pp || *e2pp) {
	    rc = 1 ;
	    if (*e1pp) {
		rc = -1 ;
	        if (*e2pp) {
	            TIMEOUT	*i1p = (TIMEOUT *) *e1pp ;
	            TIMEOUT	*i2p = (TIMEOUT *) *e2pp ;
	            rc = (i1p->val - i2p->val) ;
	        }
	    }
	}
	return rc ;
}
/* end subroutine (ourcmp) */



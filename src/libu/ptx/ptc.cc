/* ptc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* POSIX® Thread Condition manipulation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	ptc

	Description:
	This is a cleaned up version of the p-threads condition-variable
	locking facility.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<pthread.h>
#include	<ctime>			/* for |TIMESPEC| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usupport.h>
#include	<errtimer.hh>
#include	<localmisc.h>

#include	"ptca.h"
#include	"ptc.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */

static sysret_t std_ptcinit(ptc *op,ptca *ap) noex {
    	int		rs ;
	if ((rs = pthread_cond_init(op,ap)) > 0) {
	    rs = (- rs) ;
	}
	return rs ;
} /* end subroutine (std_ptcinit) */


/* local variables */


/* exported variables */


/* exported subroutines */

int ptc_create(ptc *op,ptca *ap) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    errtimer	to_nomem	= utimeout[uto_nomem] ;
	    errtimer	to_nobufs	= utimeout[uto_nobufs] ;
	    errtimer	to_again	= utimeout[uto_again] ;
	    errtimer	to_busy		= utimeout[uto_busy] ;
	    errtimer	to_io		= utimeout[uto_io] ;
	    reterr	r ;
	    repeat {
		if ((rs = std_ptcinit(op,ap)) < 0) {
                    r(rs) ;		/* <- default causes exit */
                    switch (rs) {
                    case SR_NOMEM:
                        r = to_nomem(rs) ;
                        break ;
                    case SR_NOBUFS:
                        r = to_nobufs(rs) ;
                        break ;
                    case SR_AGAIN:
                        r = to_again(rs) ;
                        break ;
                    case SR_BUSY:
                        r = to_busy(rs) ;
                        break ;
                    case SR_IO:
                        r = to_io(rs) ;
                        break ;
                    case SR_INTR:
                        r(false) ;
                        break ;
                    } /* end switch */
                    rs = r ;
	        } /* end if (error) */
	    } until ((rs >= 0) || r.fexit) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptc_create) */

int ptc_destroy(ptc *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = pthread_cond_destroy(op)) > 0) {
	        rs = (- rs) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptc_destroy) */

int ptc_broadcast(ptc *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = pthread_cond_broadcast(op)) > 0) {
	        rs = (- rs) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptc_broadcast) */

int ptc_signal(ptc *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = pthread_cond_signal(op)) > 0) {
	        rs = (- rs) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptc_signal) */

int ptc_wait(ptc *op,ptm *mp) noex {
	int		rs = SR_FAULT ;
	if (op && mp) {
	    repeat {
	        if ((rs = pthread_cond_wait(op,mp)) > 0) {
	            rs = (- rs) ;
	        }
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptc_wait) */

int ptc_waiter(ptc *op,ptm *mp,int to) noex {
	int		rs = SR_FAULT ;
	if (op && mp) {
	    if (to >= 0) {
	        TIMESPEC	ts ;
	        const clockid_t	cid = CLOCK_REALTIME ;
	        clock_gettime(cid,&ts) ;
	        ts.tv_sec += to ;
	        rs = ptc_timedwait(op,mp,&ts) ;
	    } else {
	        rs = ptc_wait(op,mp) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptc_waiter) */

int ptc_timedwait(ptc *op,ptm *mp,CTIMESPEC *tp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if (tp) {
		repeat {
	            if ((rs = pthread_cond_timedwait(op,mp,tp)) > 0) {
	                rs = (- rs) ;
	            }
		} until (rs != SR_INTR) ;
	    } else {
	        rs = ptc_wait(op,mp) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptc_timedwait) */

int ptc_reltimedwaitnp(ptc *op,ptm *mp,CTIMESPEC *tp) noex {
	int		rs = SR_NOSYS ;
	if (op) {
	    if (tp) {
	        if (syshas.reltimedwait) {
	            if ((rs = pthread_cond_reltimedwait_np(op,mp,tp)) > 0) {
	                rs = (- rs) ;
	            }
	        }
	    } else {
	        rs = ptc_wait(op,mp) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptc_reltimedwaitnp) */


/* local subroutines */

int ptc::wait(ptm *pop,int to) noex {
	return ptc_waiter(this,pop,to) ;
}

int ptc::timedwait(ptm *pop,CTIMESPEC *tsp) noex {
	return ptc_timedwait(this,pop,tsp) ;
}

int ptc::reltimedwaitnp(ptm *pop,CTIMESPEC *tsp) noex {
	return ptc_reltimedwaitnp(this,pop,tsp) ;
}

void ptc::dtor() noex {
	if (cint rs = destroy ; rs < 0) {
	    ulogerror("ptc",rs,"dtor-destroy") ;
	}
} 
/* end method (ptc::dtor) */

int ptc_creater::operator () (ptca *ap) noex {
	int	rs = SR_BUGCHECK ;
	if (op) {
	    rs = ptc_create(op,ap) ;
	}
	return rs ;
}
/* end method (ptc_creater::operator) */

int ptc_co::operator () (int) noex {
	int	rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case ptcmem_destroy:
	        rs = ptc_destroy(op) ;
	        break ;
	    case ptcmem_broadcast:
	        rs = ptc_broadcast(op) ;
	        break ;
	    case ptcmem_signal:
	        rs = ptc_signal(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (ptc_co::operator) */



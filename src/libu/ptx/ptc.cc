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
#include	<localmisc.h>

#include	"ptca.h"
#include	"ptc.h"


/* local defines */

#if	defined(SYSHAS_RELTIMEDWAIT) && ( (SYSHAS_RELTIMEDWAIT > 0))
#define	F_RELTIMEDWAIT		1	/* relative timed wait */
#else
#define	F_RELTIMEDWAIT		0	/* relative timed wait */
#endif


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */

constexpr bool		f_reltimedwait = F_RELTIMEDWAIT ;


/* exported variables */


/* exported subroutines */

int ptc_create(ptc *op,ptca *ap) noex {
	int		to_nomem = utimeout[uto_nomem] ;
	int		to_again = utimeout[uto_again] ;
	int		rs = SR_FAULT ;
	bool		f_exit = false ;
	if (op) {
	    repeat {
	        if ((rs = pthread_cond_init(op,ap)) > 0) {
		    rs = (- rs) ;
	        }
	        if (rs < 0) {
	            switch (rs) {
	            case SR_NOMEM:
	                if (to_nomem-- > 0) {
		            msleep(1000) ;
		        } else {
	                    f_exit = true ;
		        }
	                break ;
	            case SR_AGAIN:
	                if (to_again-- > 0) {
		            msleep(1000) ;
		        } else {
	                    f_exit = true ;
		        }
	                break ;
		    default:
		        f_exit = true ;
		        break ;
	            } /* end switch */
	        } /* end if (error) */
	    } until ((rs >= 0) || f_exit) ;
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
	    if ((rs = pthread_cond_wait(op,mp)) > 0) {
	        rs = (- rs) ;
	    }
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
	        if ((rs = pthread_cond_timedwait(op,mp,tp)) > 0) {
	            rs = (- rs) ;
	        }
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
	        if_constexpr (f_reltimedwait) {
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



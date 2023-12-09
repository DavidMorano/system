/* ptc */
/* lang=C++20 */

/* POSIX® Thread Condition manipulation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is a cleaned up version of the p-threads condition-variable
	locking facility.


*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<pthread.h>
#include	<time.h>		/* for |TIMESPEC| */
#include	<usystem.h>
#include	<localmisc.h>
#include	<usupport.h>

#include	"ptca.h"
#include	"ptc.h"


/* local defines */

#if	defined(SYSHAS_RELTIMEDWAIT) && ( (SYSHAS_RELTIMEDWAIT > 0))
#define	F_RELTIMEDWAIT		1	/* relative timed wait */
#else
#define	F_RELTIMEDWAIT		0	/* relative timed wait */
#endif


/* external subroutines */


/* forward references */


/* local variables */

constexpr bool		f_reltimedwait = F_RELTIMEDWAIT ;


/* exported subroutines */

int ptc_create(ptc *op,ptca *ap) noex {
	pthread_cond_t	*pop = static_cast<pthread_cond_t *>(op) ;
	int		to_nomem = utimeout[uto_nomem] ;
	int		to_again = utimeout[uto_again] ;
	int		rs ;
	bool		f_exit = false ;
	repeat {
	    if ((rs = pthread_cond_init(pop,ap)) > 0) rs = (- rs) ;
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
	return rs ;
}
/* end subroutine (ptc_create) */

int ptc_destroy(ptc *op) noex {
	pthread_cond_t	*pop = static_cast<pthread_cond_t *>(op) ;
	int		rs ;
	if ((rs = pthread_cond_destroy(pop)) > 0) {
	    rs = (- rs) ;
	}
	return rs ;
}
/* end subroutine (ptc_destroy) */

int ptc_broadcast(ptc *op) noex {
	pthread_cond_t	*pop = static_cast<pthread_cond_t *>(op) ;
	int		rs ;
	if ((rs = pthread_cond_broadcast(pop)) > 0) {
	    rs = (- rs) ;
	}
	return rs ;
}
/* end subroutine (ptc_broadcast) */

int ptc_signal(ptc *op) noex {
	pthread_cond_t	*pop = static_cast<pthread_cond_t *>(op) ;
	int		rs ;
	if ((rs = pthread_cond_signal(pop)) > 0) {
	    rs = (- rs) ;
	}
	return rs ;
}
/* end subroutine (ptc_signal) */

int ptc_wait(ptc *op,ptm *mp) noex {
	pthread_cond_t	*pop = static_cast<pthread_cond_t *>(op) ;
	int		rs ;
	if ((rs = pthread_cond_wait(pop,mp)) > 0) {
	    rs = (- rs) ;
	}
	return rs ;
}
/* end subroutine (ptc_wait) */

int ptc_waiter(ptc *op,ptm *mp,int to) noex {
	int		rs ;
	if (to >= 0) {
	    TIMESPEC		ts ;
	    const clockid_t	cid = CLOCK_REALTIME ;
	    clock_gettime(cid,&ts) ;
	    ts.tv_sec += to ;
	    rs = ptc_timedwait(op,mp,&ts) ;
	} else {
	    rs = ptc_wait(op,mp) ;
	}
	return rs ;
}
/* end subroutine (ptc_waiter) */

int ptc_timedwait(ptc *op,ptm *mp,CTIMESPEC *tp) noex {
	int		rs ;
	if (tp) {
	    pthread_cond_t	*pop = static_cast<pthread_cond_t *>(op) ;
	    if ((rs = pthread_cond_timedwait(pop,mp,tp)) > 0) {
	        rs = (- rs) ;
	    }
	} else {
	    rs = ptc_wait(op,mp) ;
	}
	return rs ;
}
/* end subroutine (ptc_timedwait) */

int ptc_reltimedwaitnp(ptc *op,ptm *mp,CTIMESPEC *tp) noex {
	int		rs = SR_NOSYS ;
	if (tp) {
	    if constexpr (f_reltimedwait) {
		pthread_cond_t	*pop = static_cast<pthread_cond_t *>(op) ;
	        if ((rs = pthread_cond_reltimedwait_np(pop,mp,tp)) > 0) {
	            rs = (- rs) ;
	        }
	    }
	} else {
	    rs = ptc_wait(op,mp) ;
	}
	return rs ;
}
/* end subroutine (ptc_reltimedwaitnp) */


/* local subroutines */

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
	return rs ;
}
/* end method (ptc_co::operator) */

int ptc::wait(ptm *pop,int to) noex {
	return ptc_waiter(this,pop,to) ;
}

int ptc::timedwait(ptm *pop,CTIMESPEC *tsp) noex {
	return ptc_timedwait(this,pop,tsp) ;
}

int ptc::reltimedwaitnp(ptm *pop,CTIMESPEC *tsp) noex {
	return ptc_reltimedwaitnp(this,pop,tsp) ;
}



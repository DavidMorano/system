/* ptm SUPPORT */
/* lang=C++20 */

/* POSIX® Thread (Pthread) Mutex manipulation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is a cleaned up version of the p-threads mutex locking facility.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<pthread.h>
#include	<climits>
#include	<usystem.h>
#include	<utimeout.h>
#include	<localmisc.h>

#include	"ptm.h"
#include	"ptma.h"


/* local defines */


/* external subroutines */


/* forward references */

int		ptm_lockto(ptm *,int) noex ;


/* local structures */

namespace {
    struct ucptm ;
    typedef int (ucptm::*mem_f)(ptm *) ;
    struct ucptm {
	mem_f		m ;
	ptma		*ap ;
	int		mto = 0 ;
	ucptm() = default ;
	ucptm(ptma *p) noex : ap(p) { } ;
	int create(ptm *) noex ;
	int lock(ptm *) noex ;
	int locktry(ptm *) noex ;
	int operator () (ptm *) noex ;
    } ; /* end struct (ucptm) */
}


/* local variables */


/* exported subroutines */

int ptm_create(ptm *op,ptma *ap) noex {
	ucptm		pmo(ap) ;
	pmo.m = &ucptm::create ;
	return pmo(op) ;
}
/* end subroutine (ptm_create) */

int ptm_destroy(ptm *op) noex {
	int		rs ;
	rs = pthread_mutex_destroy(op) ;
	if (rs > 0) rs = (- rs) ;
	return rs ;
}
/* end subroutine (ptm_destroy) */

int ptm_setprioceiling(ptm *op,int npri,int *oldp) noex {
	int		rs ;
	rs = pthread_mutex_setprioceiling(op,npri,oldp) ;
	if (rs > 0) rs = (- rs) ;
	return rs ;
}
/* end subroutine (ptm_setprioceiling) */

int ptm_getprioceiling(ptm *op,int *oldp) noex {
	int		rs ;
	rs = pthread_mutex_getprioceiling(op,oldp) ;
	if (rs > 0) rs = (- rs) ;
	return rs ;
}
/* end subroutine (ptm_getprioceiling) */

int ptm_lock(ptm *op) noex {
	ucptm		pmo ;
	pmo.m = &ucptm::lock ;
	return pmo(op) ;
}
/* end subroutine (ptm_lock) */

int ptm_lockto(ptm *op,int to) noex {
	ucptm		pmo ;
	if (to >= 0) {
	    pmo.mto = int((to * 1000) & INT_MAX) ;
	    pmo.m = &ucptm::locktry ;
	} else {
	    pmo.m = &ucptm::lock ;
	}
	return pmo(op) ;
}
/* end subroutine (ptm_lockto) */

int ptm_locktry(ptm *op) noex {
	ucptm		pmo ;
	pmo.m = &ucptm::locktry ;
	return pmo(op) ;
}
/* end subroutine (ptm_locktry) */

int ptm_unlock(ptm *op) noex {
	int		rs ;
	rs = pthread_mutex_unlock(op) ;
	if (rs > 0) rs = (- rs) ;
	return rs ;
}
/* end subroutine (ptm_unlock) */


/* local subroutines */

int ucptm::operator () (ptm *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    int		to_nomem = utimeout[uto_nomem] ;
	    int		to_again = utimeout[uto_again] ;
	    bool	f_exit = false ;
	    repeat {
	        if ((rs = (this->*m)(op)) < 0) {
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
		    case SR_BUSY:
		        if (mto-- > 0) {
	    		    msleep(1) ;
		        } else {
			    f_exit = true ;
		        }
		        break ;
		    case SR_INTR:
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
/* end subroutine (ucptm::operator) */

int ucptm::create(ptm *op) noex {
	int		rs = SR_FAULT ;
	if (ap) {
	    if ((rs = pthread_mutex_init(op,ap)) > 0) rs = (- rs) ;
	}
	return rs ;
}
/* end method (ucptm::create) */

int ucptm::lock(ptm *op) noex {
	int		rs ;
	if ((rs = pthread_mutex_lock(op)) > 0) rs = (- rs) ;
	return rs ;
}
/* end subroutine (ucptm::lock) */

int ucptm::locktry(ptm *op) noex {
	int		rs ;
	if ((rs = pthread_mutex_trylock(op)) > 0) rs = (- rs) ;
	return rs ;
}
/* end method (ucptm::locktry) */

void ptm::dtor() noex {
	int		rs = destroy ;
	if (rs < 0) {
	    ulogerror("ptm",rs,"dtor-destroy") ;
	}
} 
/* end method (ptm::dtor) */

int ptm_creater::operator () (ptma *ap) noex {
	int	rs = SR_BUGCHECK ;
	if (op) {
	    rs = ptm_create(op,ap) ;
	}
	return rs ;
}
/* end method (ptm_creater::operator) */

int ptm_co::operator () (int to) noex {
	int	rs = SR_BUGCHECK ;
	switch (w) {
	case ptmmem_destroy:
	    rs = ptm_destroy(op) ;
	    break ;
	case ptmmem_lockbegin:
	    rs = ptm_lockto(op,to) ;
	    break ;
	case ptmmem_lockend:
	    rs = ptm_unlock(op) ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end method (ptm_co::operator) */



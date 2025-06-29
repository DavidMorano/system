/* ptm SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* POSIX® Thread (Pthread) Mutex manipulation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	ptm

	Description:
	This is a cleaned up version of the p-threads mutex locking facility.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<pthread.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<climits>		/* |INT_MAX| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usupport.h>
#include	<errtimer.hh>
#include	<localmisc.h>

#include	"ptm.h"
#include	"ptma.h"


/* local defines */


/* external subroutines */


/* forward references */


/* local structures */

namespace {
    struct ucptm ;
    typedef int (ucptm::*mem_m)(ptm *) ;
    struct ucptm {
	mem_m		m ;
	ptma		*ap ;
	int		mto = 0 ;
	ucptm() = default ;
	ucptm(ptma *p) noex : ap(p) { } ;
	int create(ptm *) noex ;
	int lock(ptm *) noex ;
	int locktry(ptm *) noex ;
	int operator () (ptm *) noex ;
    } ; /* end struct (ucptm) */
} /* end namespace */


/* local variables */


/* exported variables */


/* exported subroutines */

int ptm_create(ptm *op,ptma *ap) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    ucptm	pmo(ap) ;
	    pmo.m = &ucptm::create ;
	    if ((rs = pmo(op)) >= 0) {
		op->fl.open = true ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptm_create) */

int ptm_destroy(ptm *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = pthread_mutex_destroy(op)) > 0) {
	        rs = (- rs) ;
	    }
	    op->fl.open = false ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptm_destroy) */

int ptm_setprioceiling(ptm *op,int npri,int *oldp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = pthread_mutex_setprioceiling(op,npri,oldp)) > 0) {
	        rs = (- rs) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptm_setprioceiling) */

int ptm_getprioceiling(ptm *op,int *oldp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = pthread_mutex_getprioceiling(op,oldp)) > 0) {
	        rs = (- rs) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptm_getprioceiling) */

int ptm_lockbegin(ptm *op) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    ucptm	pmo ;
	    pmo.m = &ucptm::lock ;
	    rs = pmo(op) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptm_lockbegin) */

int ptm_lockbeginto(ptm *op,int to) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    ucptm	pmo ;
	    if (to >= 0) {
	        pmo.mto = int((to * 1000) & INT_MAX) ;
	        pmo.m = &ucptm::locktry ;
	    } else {
	        pmo.m = &ucptm::lock ;
	    }
	    rs = pmo(op) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptm_lockbeginto) */

int ptm_lockbegintry(ptm *op) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    ucptm	pmo ;
	    pmo.m = &ucptm::locktry ;
	    rs = pmo(op) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptm_lockbegintry) */

int ptm_lockend(ptm *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = pthread_mutex_unlock(op)) > 0) {
	        rs = (- rs) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptm_lockend) */


/* local subroutines */

int ucptm::operator () (ptm *op) noex {
	errtimer	to_nomem	= utimeout[uto_nomem] ;
	errtimer	to_nobufs	= utimeout[uto_nobufs] ;
	errtimer	to_again	= utimeout[uto_again] ;
	errtimer	to_busy		= utimeout[uto_busy] ;
	errtimer	to_io		= utimeout[uto_io] ;
	reterr		r ;
	int		rs ;
	repeat {
	    if ((rs = (this->*m)(op)) < 0) {
                r(rs) ;                 /* <- default causes exit */
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
	return rs ;
}
/* end subroutine (ucptm::operator) */

int ucptm::create(ptm *op) noex {
	int		rs ;
	if ((rs = pthread_mutex_init(op,ap)) > 0) {
	    rs = (- rs) ;
	}
	return rs ;
}
/* end method (ucptm::create) */

int ucptm::lock(ptm *op) noex {
	int		rs ;
	if ((rs = pthread_mutex_lock(op)) > 0) {
	    rs = (- rs) ;
	}
	return rs ;
}
/* end subroutine (ucptm::lock) */

int ucptm::locktry(ptm *op) noex {
	int		rs ;
	if ((rs = pthread_mutex_trylock(op)) > 0) {
	    rs = (- rs) ;
	}
	return rs ;
}
/* end method (ucptm::locktry) */

void ptm::dtor() noex {
	if (cint rs = destroy ; rs < 0) {
	    ulogerror("ptm",rs,"dtor-destroy") ;
	}
} 
/* end method (ptm::dtor) */

int ptm_creater::operator () (ptma *ap) noex {
	return ptm_create(op,ap) ;
}
/* end method (ptm_creater::operator) */

int ptm_co::operator () (int to) noex {
	int	rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case ptmmem_destroy:
	        rs = ptm_destroy(op) ;
	        break ;
	    case ptmmem_lockbegin:
	        rs = ptm_lockbeginto(op,to) ;
	        break ;
	    case ptmmem_lockend:
	        rs = ptm_lockend(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (ptm_co::operator) */



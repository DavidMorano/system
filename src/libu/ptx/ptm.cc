/* ptm SUPPORT (POSIX®-Thread-Mutex) */
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
#include	<pthread.h>		/* POSIX® */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<numeric>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU |POLL_INTMULT| */
#include	<usyscalls.h>		/* LIBU |um(3u)| */
#include	<usupport.h>		/* LIBU |msleep(3u)| */
#include	<errtimer.hh>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ptma.h"
#include	"ptm.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct ptmer ;
    typedef int (ptmer::*ptmer_m)(ptm *) noex ;
    struct ptmer {
	ptmer_m		m ;
	ptma		*ap ;
	int		mto = 0 ;
	ptmer() = default ;
	ptmer(ptma *p) noex : ap(p) { } ;
	int create	(ptm *) noex ;
	int lock	(ptm *) noex ;
	int locktry	(ptm *) noex ;
	int operator () (ptm *) noex ;
    } ; /* end struct (ptmer) */
} /* end namespace */


/* forward references */

template<typename ... Args>
local inline int ptm_magic(ptm *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == PTM_MAGIC) ? SR_OK : SR_NOTOPEN ;
	} /* end */
	return rs ;
} /* end subroutine (ptm_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int ptm_create(ptm *op,ptma *ap) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    ptmer	pmo(ap) ;
	    pmo.m = &ptmer::create ;
	    rs = pmo(op) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ptm_create) */

int ptm_destroy(ptm *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    repeat {
	        if ((rs = pthread_mutex_destroy(op)) > 0) {
	            rs = (neg rs) ;
	        } else if (rs < 0) {
		    rs = SR_NOANODE ;
	        }
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ptm_destroy) */

int ptm_setprioceiling(ptm *op,int npri,int *oldp) noex {
	int		rs = SR_FAULT ;
	if (op && oldp) ylikely {
	    repeat {
	        if ((rs = pthread_mutex_setprioceiling(op,npri,oldp)) > 0) {
	            rs = (neg rs) ;
	        } else if (rs < 0) {
		    rs = SR_NOANODE ;
	        }
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ptm_setprioceiling) */

int ptm_getprioceiling(ptm *op,int *oldp) noex {
	int		rs = SR_FAULT ;
	if (op && oldp) ylikely {
	    repeat {
	        if ((rs = pthread_mutex_getprioceiling(op,oldp)) > 0) {
	            rs = (neg rs) ;
	        } else if (rs < 0) {
		    rs = SR_NOANODE ;
	        }
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ptm_getprioceiling) */

int ptm_lockbeg(ptm *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    ptmer	pmo ;
	    pmo.m = &ptmer::lock ;
	    rs = pmo(op) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ptm_lockbeg) */

int ptm_lockbegto(ptm *op,int to) noex {
    	cint		rsb = SR_BUSY ;
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if (to > 0) {
		int mto = sat_mul(to,POLL_INTMULT) ;
		rs = SR_OK ;
		for (int c = 0 ; mto ; c += 1) {
		    if (c) {
			rs = msleep(1) ;
		    }
		    if (rs >= 0) {
		        rs = ptm_locktry(op) ;
		    } /* end */
		    if (rs != rsb) break ;
		    mto -= 1 ;
		} /* end for */
		if ((rs == rsb) && (mto == 0)) {
		    rs = SR_TIMEDOUT ;
		} /* end */
	    } else if (to == 0) {
		if ((rs = ptm_locktry(op)) == rsb) {
		    rs = SR_TIMEDOUT ;
		} /* end */
	    } else {
		rs = ptm_lockbeg(op) ;
	    } /* end */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ptm_lockbegto) */

int ptm_locktry(ptm *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    ptmer	pmo ;
	    pmo.m = &ptmer::locktry ;
	    rs = pmo(op) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ptm_locktry) */

int ptm_lockend(ptm *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    repeat {
	        if ((rs = pthread_mutex_unlock(op)) > 0) {
	            rs = (neg rs) ;
	        } else if (rs < 0) {
		    rs = SR_NOANODE ;
	        } /* end */
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ptm_lockend) */


/* local subroutines */

int ptmer::operator () (ptm *op) noex {
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
} /* end subroutine (ptmer::operator) */

int ptmer::create(ptm *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if ((rs = pthread_mutex_init(op,ap)) > 0) {
	        rs = (neg rs) ;
	    } else if (rs < 0) {
		rs = SR_NOANODE ;
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
} /* end method (ptmer::create) */

int ptmer::lock(ptm *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if ((rs = pthread_mutex_lock(op)) > 0) {
	        rs = (neg rs) ;
	    } else if (rs < 0) {
		rs = SR_NOANODE ;
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ptmer::lock) */

int ptmer::locktry(ptm *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if ((rs = pthread_mutex_trylock(op)) > 0) {
	        rs = (neg rs) ;
	    } else if (rs < 0) {
		rs = SR_NOANODE ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end method (ptmer::locktry) */

void ptm::dtor() noex {
	if (cint rs = destroy ; rs < 0) {
	    ulogerror("ptm",rs,"dtor-destroy") ;
	} /* end */
} /* end method (ptm::dtor) */

int ptm_creater::operator () (ptma *ap) noex {
    	int		rs ;
	if ((rs = ptm_create(op,ap)) >= 0) ylikely {
	    op->magval = PTM_MAGIC ;
	} /* end */
	return rs ;
} /* end method (ptm_creater::operator) */

int ptm_co::operator () (int to) noex {
    	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case ptmsub_destroy:
	        rs = ptm_destroy(op) ;
	    	op->magval = 0 ;
	        break ;
	    case ptmsub_lockbeg:
	        rs = ptm_lockbegto(op,to) ;
	        break ;
	    case ptmsub_lockend:
	        rs = ptm_lockend(op) ;
	        break ;
	    } /* end switch */
	} /* end if (magic) */
	return rs ;
} /* end method (ptm_co::operator) */



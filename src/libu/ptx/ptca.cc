/* ptca SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* POSIX® Thread Condition Attribute manipulation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	ptca

	Description:
	This is a cleaned up version of the p-threads condition-variable
	attribute set of subroutines (object).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<pthread.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usupport.h>
#include	<errtimer.hh>
#include	<localmisc.h>

#include	"ptca.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int ptca_create(ptca *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    errtimer	to_nomem	= utimeout[uto_nomem] ;
	    errtimer	to_nobufs       = utimeout[uto_nobufs] ;
	    errtimer	to_again	= utimeout[uto_again] ;
	    errtimer	to_busy         = utimeout[uto_busy] ;
	    reterr	r ;
	    repeat {
	        if ((rs = pthread_condattr_init(op)) > 0) {
		    rs = (- rs) ;
		    r(rs) ;
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
		    case SR_INTR:
			r(false) ;
		        break ;
	            } /* end switch */
		    rs = r ;
		} else if (rs < 0) {
		    rs = SR_NOANODE ;
		    r(true) ;
	        } /* end if (error) */
	    } until ((rs >= 0) || r.fexit) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptca_create) */

int ptca_destroy(ptca *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if ((rs = pthread_condattr_destroy(op)) > 0) {
	        rs = (- rs) ;
	    } else if (rs < 0) {
		rs = SR_NOANODE ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptca_destroy) */

int ptca_getpshared(ptca *op,int *oldp) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if ((rs = pthread_condattr_getpshared(op,oldp)) > 0) {
	        rs = (- rs) ;
	    } else if (rs < 0) {
		rs = SR_NOANODE ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptca_getpshared) */

int ptca_setpshared(ptca *op,int fl) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if ((rs = pthread_condattr_setpshared(op,fl)) > 0) {
	        rs = (- rs) ;
	    } else if (rs < 0) {
		rs = SR_NOANODE ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptca_setpshared) */


/* local subroutines */

void ptca::dtor() noex {
	if (cint rs = ptca_destroy(this) ; rs < 0) {
	    ulogerror("ptca",rs,"dtor-destroy") ;
	}
} /* end method (ptca::dtor) */

int ptca::getpshared(int *rp) noex {
	return ptca_getpshared(this,rp) ;
}

int ptca_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case ptcamem_create:
	        if ((rs = ptca_create(op)) >= 0) ylikely {
		    op->magic = PTCA_MAGIC ;
		}
	        break ;
	    case ptcamem_destroy:
	        rs = ptca_destroy(op) ;
		op->magic = 0 ;
	        break ;
	    case ptcamem_setpshared:
	        rs = ptca_setpshared(op,a) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (ptca_co::operator) */



/* ptca SUPPORT */
/* lang=C++20 */

/* POSIX® Thread Condition Attribute manipulation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is a cleaned up version of the p-threads condition-variable
	attribute set of subroutines (object).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<pthread.h>
#include	<usystem.h>
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
	if (op) {
	    int		to_nomem = utimeout[uto_nomem] ;
	    bool	f_exit = false ;
	    repeat {
	        if ((rs = pthread_condattr_init(op)) > 0) {
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
/* end subroutine (ptca_create) */

int ptca_destroy(ptca *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = pthread_condattr_destroy(op)) > 0) {
	        rs = (- rs) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptca_destroy) */

int ptca_getpshared(ptca *op,int *oldp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = pthread_condattr_getpshared(op,oldp)) > 0) {
	        rs = (- rs) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptca_getpshared) */

int ptca_setpshared(ptca *op,int fl) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = pthread_condattr_setpshared(op,fl)) > 0) {
	        rs = (- rs) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptca_setpshared) */


/* local subroutines */

void ptca::dtor() noex {
	int		rs = ptca_destroy(this) ;
	if (rs < 0) {
	    ulogerror("ptca",rs,"dtor-destroy") ;
	}
} 
/* end method (ptca::dtor) */

int ptca_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case ptcamem_create:
	        rs = ptca_create(op) ;
	        break ;
	    case ptcamem_destroy:
	        rs = ptca_destroy(op) ;
	        break ;
	    case ptcamem_setpshared:
	        rs = ptca_setpshared(op,a) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (ptca_co::operator) */

int ptca::getpshared(int *rp) noex {
	return ptca_getpshared(this,rp) ;
}



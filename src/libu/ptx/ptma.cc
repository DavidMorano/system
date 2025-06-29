/* ptma SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* POSIX® Thread Mutex Attribute manipulation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	ptma

	Description:
	This is a cleaned up version of the p-threads mutex-attribute
	set of subroutines (object).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<pthread.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usupport.h>
#include	<localmisc.h>

#include	"ptma.h"


/* local defines */

#if	defined(SYSHAS_MUTEXROBUST) && (SYSHAS_MUTEXROBUST > 0)
#define	F_MUTEXROBUST	1
#else
#define	F_MUTEXROBUST	0
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr bool	f_mutexrobust = F_MUTEXROBUST ;


/* exported variables */


/* exported subroutines */

int ptma_create(ptma *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    int		to_nomem = utimeout[uto_nomem] ;
	    bool	f_exit = false ;
	    repeat {
	        if ((rs = pthread_mutexattr_init(op)) > 0) {
		    rs = (- rs) ;
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
	    if (rs >= 0) {
	        op->fl.open = true ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptma_create) */

int ptma_destroy(ptma *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = pthread_mutexattr_destroy(op)) > 0) {
	        rs = (- rs) ;
	    }
	    op->fl.open = false ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptma_destroy) */

int ptma_getprioceiling(ptma *op,int *oldp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = pthread_mutexattr_getprioceiling(op,oldp)) > 0) {
	        rs = (- rs) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptma_getprioceiling) */

int ptma_setprioceiling(ptma *op,int fn) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = pthread_mutexattr_setprioceiling(op,fn)) > 0) {
	        rs = (- rs) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptma_setprioceiling) */

int ptma_getprotocol(ptma *op,int *oldp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = pthread_mutexattr_getprotocol(op,oldp)) > 0) {
	        rs = (- rs) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptma_getprotocol) */

int ptma_setprotocol(ptma *op,int fn) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = pthread_mutexattr_setprotocol(op,fn)) > 0) {
	        rs = (- rs) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptma_setprotocol) */

int ptma_getpshared(ptma *op,int *oldp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = pthread_mutexattr_getpshared(op,oldp)) > 0) {
	        rs = (- rs) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptma_getpshared) */

int ptma_setpshared(ptma *op,int fn) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = pthread_mutexattr_setpshared(op,fn)) > 0) {
	        rs = (- rs) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptma_setpshared) */

int ptma_getrobustnp(ptma *op,int *oldp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if_constexpr (f_mutexrobust) {
	        if ((rs = pthread_mutexattr_getrobust_np(op,oldp)) > 0) {
	            rs = (- rs) ;
		}
	    } else {
	        (void) oldp ;
	        rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptma_getrobustnp) */

int ptma_setrobustnp(ptma *op,int fn) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if_constexpr (f_mutexrobust) {
		if ((rs = pthread_mutexattr_setrobust_np(op,fn)) > 0) {
		    rs = (- rs) ;
		}
	    } else {
	        (void) fn ;
	        rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptma_setrobustnp) */

int ptma_gettype(ptma *op,int *oldp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = pthread_mutexattr_gettype(op,oldp)) > 0) {
	        rs = (- rs) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptma_gettype) */

int ptma_settype(ptma *op,int nf) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = pthread_mutexattr_settype(op,nf)) > 0) {
	        rs = (- rs) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptma_settype) */


/* local subroutines */

void ptma::dtor() noex {
	if (cint rs = ptma_destroy(this) ; rs < 0) {
	    ulogerror("ptma",rs,"dtor-destroy") ;
	}
} 
/* end method (ptma::dtor) */

int ptma_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case ptmamem_create:
	        rs = ptma_create(op) ;
	        break ;
	    case ptmamem_destroy:
	        rs = ptma_destroy(op) ;
	        break ;
	    case ptmamem_setprioceiling:
	        rs = ptma_setprioceiling(op,a) ;
	        break ;
	    case ptmamem_setprotocol:
	        rs = ptma_setprotocol(op,a) ;
	        break ;
	    case ptmamem_setpshared:
	        rs = ptma_setpshared(op,a) ;
	        break ;
	    case ptmamem_setrobustnp:
	        rs = ptma_setrobustnp(op,a) ;
	        break ;
	    case ptmamem_settype:
	        rs = ptma_settype(op,a) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (ptma_co::operator) */

int ptma::getprioceiling(int *rp) noex {
	return ptma_getprioceiling(this,rp) ;
}

int ptma::getprotocol(int *rp) noex {
	return ptma_getprotocol(this,rp) ;
}

int ptma::getpshared(int *rp) noex {
	return ptma_getpshared(this,rp) ;
}

int ptma::getrobustnp(int *rp) noex {
	return ptma_getrobustnp(this,rp) ;
}

int ptma::gettype(int *rp) noex {
	return ptma_gettype(this,rp) ;
}



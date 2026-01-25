/* ptrwa SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* POSIX® Thread Read-Write lock attribute manipulation */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	ptrwa

  	Description:
	This little object provides operational attributes for the
	POSIX read-write lock.

	Side-note: 
	The POXIX® read-write lock is broken on the Solaris® platform
	and is not used any longer.  So this object here gets little
	(no) use now-a-days (because of the broken Solaris®
	read-write lock implementation).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<pthread.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usupport.h>
#include	<errtimer.hh>
#include	<localmisc.h>

#include	"ptrwa.h"


/* local defines */


/* external subroutines */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int ptrwa_create(ptrwa *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    errtimer	to_nomem	= utimeout[uto_nomem] ;
	    errtimer	to_nobufs       = utimeout[uto_nobufs] ;
	    errtimer	to_again	= utimeout[uto_again] ;
	    errtimer	to_busy         = utimeout[uto_busy] ;
	    reterr	r ;
	    repeat {
	        if ((rs = pthread_rwlockattr_init(op)) > 0) {
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
/* end subroutine (ptrwa_create) */

int ptrwa_destroy(ptrwa *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if ((rs = pthread_rwlockattr_destroy(op)) > 0) {
	        rs = (- rs) ;
	    } else if (rs < 0) {
		rs = SR_NOANODE ;
	    }
	}
	return rs ;
}
/* end subroutine (ptrwa_destroy) */

int ptrwa_getpshared(ptrwa *op,int *oldp) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if ((rs = pthread_rwlockattr_getpshared(op,oldp)) > 0) {
	        rs = (- rs) ;
	    } else if (rs < 0) {
		rs = SR_NOANODE ;
	    }
	}
	return rs ;
}
/* end subroutine (ptrwa_getpshared) */

int ptrwa_setpshared(ptrwa *op,int fl) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if (fl < 0) fl = PTHREAD_PROCESS_SHARED ;
	    if ((rs = pthread_rwlockattr_setpshared(op,fl)) > 0) {
	        rs = (- rs) ;
	    } else if (rs < 0) {
		rs = SR_NOANODE ;
	    }
	}
	return rs ;
}
/* end subroutine (ptrwa_setpshared) */

int ptrwa::getpshared(int *rp) noex {
    	return ptrwa_getpshared(this,rp) ;
}

void ptrwa::dtor() noex {
	if (cint rs = ptrwa_destroy(this) ; rs < 0) {
	    ulogerror("pta",rs,"dtor-destroy") ;
	}
} /* end method (ptrwa::dtor) */

int ptrwa_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case ptrwamem_create:
	        if ((rs = ptrwa_create(op)) >= 0) ylikely {
		    op->magic = PTRWA_MAGIC ;
		}
	        break ;
	    case ptrwamem_destroy:
	        rs = ptrwa_destroy(op) ;
		op->magic = 0 ;
	        break ;
	    case ptrwamem_setpshared:
	        rs = ptrwa_setpshared(op,a) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (ptrwa_co::operator) */



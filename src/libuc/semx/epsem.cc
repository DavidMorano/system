/* epsem SUPPORT (emulated POSIX® Semaphore) */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* unnamed POSIX© Semaphore (PSEM) */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-23, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	psem

	Description:
	This module provides a sanitized version of the standard
	POSIX® semaphore facility provided with some new UNIX®i.
	Some operating system problems are managed within these
	routines for the common stuff that happens when a poorly
	configured OS gets overloaded!

	Enjoy!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

#include	"epsem.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int psem_create(psem *op,int pshared,int cnt) noex {
	return csem_create(op,pshared,cnt) ;
}
/* end subroutine (psem_create) */

int psem_destroy(psem *op) noex {
	return csem_destroy(op) ;
}
/* end subroutine (psem_destroy) */

int psem_wait(psem *op) noex {
	return csem_decr(op,1,-1) ;
}
/* end subroutine (psem_wait) */

int psem_waiter(psem *op,int to) noex {
	return csem_decr(op,1,to) ;
}
/* end subroutine (psem_waiter) */

int psem_trywait(psem *op) noex {
	cint		rsto = SR_TIMEDOUT ;
	int		rs ;
	if ((rs = csem_decr(op,1,0)) == rsto) {
	    rs = SR_AGAIN ;		/* <- required by |psem(3uc)| */
	}
	return rs ;
}
/* end subroutine (psem_trywait) */

int psem_post(psem *op) noex {
	return csem_incr(op,1) ;
}
/* end subroutine (psem_post) */

int psem_count(psem *op) noex {
	return csem_count(op) ;
}

int psem::create(int pshared,int acnt) noex {
	return psem_create(this,pshared,acnt) ;
}

psem::operator int () noex {
	return csem_count(this) ;
}

void psem::dtor() noex {
	if (cint rs = destroy ; rs < 0) {
	    ulogerror("psem",rs,"fini-destroy") ;
	}
}

int psem_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case psemmem_wait:
	        rs = psem_wait(op) ;
	        break ;
	    case psemmem_waiter:
	        rs = psem_waiter(op,a) ;
	        break ;
	    case psemmem_trywait:
	        rs = psem_trywait(op) ;
	        break ;
	    case psemmem_post:
	        rs = psem_post(op) ;
	        break ;
	    case psemmem_count:
	        rs = psem_count(op) ;
	        break ;
	    case psemmem_destroy:
	        rs = psem_destroy(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (psem_co::operator) */



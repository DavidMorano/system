/* epsem SUPPORT (emulated POSIX® Semaphore) */
/* charset=ISO8859-1 */
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
	epsem

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

int epsem_create(epsem *op,int pshared,int cnt) noex {
	return csem_create(op,pshared,cnt) ;
}
/* end subroutine (epsem_create) */

int epsem_destroy(epsem *op) noex {
	return csem_destroy(op) ;
}
/* end subroutine (epsem_destroy) */

int epsem_wait(epsem *op) noex {
	return csem_decr(op,1,-1) ;
}
/* end subroutine (epsem_wait) */

int epsem_waiter(epsem *op,int to) noex {
	return csem_decr(op,1,to) ;
}
/* end subroutine (epsem_waiter) */

int epsem_trywait(epsem *op) noex {
	cint		rsto = SR_TIMEDOUT ;
	int		rs ;
	if ((rs = csem_decr(op,1,0)) == rsto) {
	    rs = SR_AGAIN ;		/* <- required by |epsem(3uc)| */
	}
	return rs ;
}
/* end subroutine (epsem_trywait) */

int epsem_post(epsem *op) noex {
	return csem_incr(op,1) ;
}
/* end subroutine (epsem_post) */

int epsem_count(epsem *op) noex {
	return csem_count(op) ;
}
/* end subroutine (epsem_count) */


/* local subroutines */

int epsem::create(int pshared,int acnt) noex {
	return epsem_create(this,pshared,acnt) ;
}

epsem::operator int () noex {
	return csem_count(this) ;
}

void epsem::dtor() noex {
	if (cint rs = destroy ; rs < 0) {
	    ulogerror("epsem",rs,"fini-destroy") ;
	}
}

int epsem_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case epsemmem_wait:
	        rs = epsem_wait(op) ;
	        break ;
	    case epsemmem_waiter:
	        rs = epsem_waiter(op,a) ;
	        break ;
	    case epsemmem_trywait:
	        rs = epsem_trywait(op) ;
	        break ;
	    case epsemmem_post:
	        rs = epsem_post(op) ;
	        break ;
	    case epsemmem_count:
	        rs = epsem_count(op) ;
	        break ;
	    case epsemmem_destroy:
	        rs = epsem_destroy(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (epsem_co::operator) */



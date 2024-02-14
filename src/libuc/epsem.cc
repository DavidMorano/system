/* psem SUPPORT */
/* lang=C++20 */

/* POSIX© unnamed Semaphore (PSEM) */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-23, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module provides a sanitized version of the standard
	POSIX® semaphore facility provided with some new UNIX®i.
	Some operating system problems are managed within these
	routines for the common stuff that happens when a poorly
	configured OS gets overloaded!

	Enjoy!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<usystem.h>
#include	<localmisc.h>

#if	defined(PSEM_REDIRECT) && (PSEM_REDIRECT > 0)
#include	"epsem.h"
#else /* redirect */
#include	"psem.h"
#endif /* redirect */


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* forward references */


/* local variables */


/* exported subroutines */

int psem_create(psem *psp,int pshared,int count) noex {
	return csem_create(psp,pshared,count) ;
}
/* end subroutine (psem_create) */

int psem_destroy(psem *psp) noex {
	return csem_destroy(psp) ;
}
/* end subroutine (psem_destroy) */

int psem_wait(psem *psp) noex {
	return csem_decr(psp,1,-1) ;
}
/* end subroutine (psem_wait) */

int psem_trywait(psem *psp) noex {
	cint		rsto = SR_TIMEDOUT ;
	int		rs ;
	if ((rs = csem_decr(psp,1,0)) == rsto) {
	    rs = SR_AGAIN ;		/* <- required by |psem(3uc)| */
	}
	return rs ;
}
/* end subroutine (psem_trywait) */

int psem_waiter(psem *psp,int to) noex {
	return csem_decr(psp,1,to) ;
}
/* end subroutine (psem_waiter) */

int psem_post(psem *psp) noex {
	return csem_incr(psp,1) ;
}
/* end subroutine (psem_post) */



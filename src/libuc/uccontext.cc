/* uccontext SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ucontext.h>		/* POSIX® «- system-header money-shot */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"uccontext.h"


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

int uc_contextmake(ucontext_t *nucp,context_f cf,int val) noex {
	int		rs = SR_FAULT ;
	if (nucp && cf) {
	    void_f	vf = void_f(cf) ;
	    makecontext(nucp,vf,1,val) ;
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_contextmake) */

int uc_contextswap(ucontext_t *oucp,const ucontext_t *nucp) noex {
	int		rs = SR_FAULT ;
	if (oucp && nucp) {
	    if ((rs = swapcontext(oucp,nucp)) < 0) {
	        rs = (neg errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_contextswap) */

int uc_contextset(ucontext_t *nucp) noex {
	int		rs = SR_FAULT ;
	if (nucp) {
	    setcontext(nucp) ;
	    rs = SR_BUGCHECK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_contextset) */



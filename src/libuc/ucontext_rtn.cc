/* ucontext_rtn SUPPORT */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* retrieve return address from UCONTEXT */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David AÂ­DÂ­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Retrives the return-address from the UCONTEXT structure.

	Q. Why the name of this subroutine?
	A. The method name 'rtn' was already used for a member of
	a register window structure on the SPARC platform, so we
	adopted it.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<ucontext.h>
#include	<usystem.h>
#include	<usysflag.h>
#include	<localmisc.h>


/* local defines */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

#if	defined(F_SUNOS) && (F_SUNOS > 0)

int ucontext_rtn(ucontext_t *ucp,long *rpp) noex {
	int		rs = SR_FAULT ;
	if (ucp && rpp) {
	    mcontext_t	*mcp = &ucp->uc_mcontext ;
	    if (mcp) {
	            greg_t	*rp = mcp->gregs ;
	            rs = SR_OK ;
	            *rpp = rp[(_NGREG-1)] ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucontext_rtn) */

#else /* defined(F_SUNOS) && (F_SUNOS > 0) */

int ucontext_rtn(ucontext_t *ucp,long *rpp) noex {
	int		rs = SR_FAULT ;
	if (ucp && rpp) {
	    mcontext_t	*mcp = &ucp->uc_mcontext ;
	    if (mcp) {
		rs = SR_NOSYS ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucontext_rtn) */

#endif /* defined(F_SUNOS) && (F_SUNOS > 0) */




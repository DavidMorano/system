/* uclstat SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* get status on a file */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<prefixfn.h>
#include	<localmisc.h>

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_lstat(cchar *fname,ustat *sbp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if (fname && sbp) {
	    cchar *fn ;
	    if (prefixfn pf ; (rs = pf.start(fname,-1,&fn)) >= 0) {
		{
	      	    rs = u_lstat(fn,sbp) ;
		    rv = rs ;
		}
		rs1 = pf.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (prefixfn) */
	} /* end if (non-null) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (uc_lstat) */



/* ucaccess SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* POSIX file-unlink */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<prefixfn.h>
#include	<localmisc.h>


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_access(cchar *fname,int am) noex {
	int		rs = SR_INVALID ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if (am >= 0) {
	    cchar *fn ;
	    if (prefixfn pf ; (rs = pf.start(fname,-1,&fn)) > 0) {
		{
		    rs = u_access(fn,am) ;
		    rv = rs ;
		}
		rs1 = pf.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (prefixfn) */
	} /* end if (valid) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (uc_access) */



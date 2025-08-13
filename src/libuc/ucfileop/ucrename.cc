/* ucrename SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* rename (link to and delete original) a file */
/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_rename

	Description:
	This is the middleware version of |u_rename(3u)|.

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<prefixfn.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* exported variables */


/* exported subroutines */

int uc_rename(cchar *fname,cchar *nfname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if (fname && nfname) {
	    rs = SR_INVALID ;
	    if (nfname[0]) {
		cchar *fn ;
		if (prefixfn pf ; (rs = pf.start(fname,-1,&fn)) > 0) {
		    {
		        rs = u_rename(fn,nfname) ;
			rv = rs ;
		    }
		    rs1 = pf.finish ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (prefixfn) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (uc_rename) */



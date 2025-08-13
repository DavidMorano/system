/* ucresolvepath SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX�® library-3c */
/* resolve a path without symbolic components */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_resolvepath

	Description:
	This subroutine takes an existing path and creates a new
	path that does not contain any symbolic components.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
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

int uc_resolvepath(cchar *fname,char *rbuf,int rlen) noex {
    	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if (fname && rbuf) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (fname[0] && (rlen > 0)) {
		cchar *fn ;
		if (prefixfn pf ; (rs = pf.start(fname,-1,&fn)) > 0) {
		    {
		        rs = u_resolvepath(fn,rbuf,rlen) ;
			rv = rs ;
		    }
		    rs1 = pf.finish ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (prefixfn) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (uc_resolvepath) */



/* ucstat SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* get status on a file */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 1998-06-03, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_stat

	Description:
	This subroutine provides an enchanced |stat(2)| function.
	It also removes trailing slash ('/') character from file-names,
	but it will preserve a lone pair of characters consisting
	of '//'.

	Notes:
	Some systems do not ignore trailing slash characters on
	file-names, so we take care of it for them (at least here
	we do).

*******************************************************************************/

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

int uc_stat(cchar *fname,ustat *sbp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		flen = 0 ; /* return-value */
	if (fname && sbp) {
	    cchar	*fn ;
	    if (prefixfn pf ; (rs = pf.start(fname,-1,&fn)) > 0) {
	        if ((rs = u_stat(fn,sbp)) >= 0) {
		    flen = intsat(sbp->st_size) ;
	        }
		rs1 = pf.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (prefixfn) */
	} /* end if (non-null) */
	return (rs >= 0) ? flen : rs ;
}
/* end subroutine (uc_stat) */



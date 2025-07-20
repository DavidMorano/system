/* ucstrtod SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* string-to-florating-point functions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<localmisc.h>

#include	"ucstrtod.h"


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

int uc_strtod(cchar *startp,char **endpp,double *rp) noex {
	int	rs = SR_FAULT ;
	if (startp && rp) ylikely {
	    char	*ep{} ;
	    errno = 0 ;
	    *rp = strtod(startp,&ep) ;
	    if (errno) {
		rs = (- errno) ;
	    } else {
		rs = intconv(ep - startp) ;
		if (endpp) *endpp = ep ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_strtod) */



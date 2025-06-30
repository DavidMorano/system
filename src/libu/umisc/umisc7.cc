/* umisc7 MODULE */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® Miscellaneous support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-19, David A­D­ Morano
	This code was originally written.

	= 2020-05-07, David A-D- Morano
	I converted this (formerly a header-only file) to a module.

*/

/* Copyright © 1998,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Module:
	umisc

	Description:
	This module provides miscellaneous UNIX® or other
	common (library oriented) subroutines.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

module umisc ;

import libutil ;

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

cchar *strbasename(cchar *s) noex {
	int		si = 0 ;
	int		sl = xstrlen(s) ;
	/* remove trailing slash characters */
	while ((sl > 1) && (s[sl - 1] == '/')) {
	    sl -= 1 ;
	}
	/* find the next previous slash character */
	for (si = sl ; si > 0 ; si -= 1) {
	    if (s[si - 1] == '/') break ;
	} /* end for */
	if (s[1] == '\0') si = 0 ;
	return (s + si) ;
}
/* end subroutine (strbasename) */



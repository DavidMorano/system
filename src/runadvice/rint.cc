/* rint SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* revision history:

	= 1995-01-06, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/*******************************************************************************

	Name:
	rint

	Description:
	Convert a double type to an integer type.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */


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

int rint(double d) noex {
	return (int) d ;
} /* end subroutine */



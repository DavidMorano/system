/* bufsizevar SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* this provides a variable with a buffer-size */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-12-03, David A­D­ Morano
	This code was born out of frustration with cleaning up bad
	legacy code (of which there is quite a bit -- like almost
	all of it).

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	bufsizevar

	Description:
	This is an object that serves as a variable to retrieve
	a buffer size of something in the system.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<getbufsize.h>
#include	<localmisc.h>

#include	"bufsizevar.hh"


/* local defines */


/* external namespaces */


/* external typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

bufsizevar::operator int () noex {
	int		rs = SR_OK ;
	if (val == 0) nlikely {
	    if ((rs = getbufsize(name)) > 0) {
		val = rs ;
	    } else if (rs == 0) {
		val = def ;
	    }
	} /* end if (needed) */
	return (rs >= 0) ? val : rs ;
}
/* end method (bufsizevar::operator) */



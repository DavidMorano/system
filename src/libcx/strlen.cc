/* strlen SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* calculate the length of a string */
/* version %I% last-modified %G% */


/* revision history:

	= 1982-09-10, David A­D­ Morano
	This subroutine was written because I need this on our own
	embedded (VMS CPU) platform.

*/

/* Copyright © 1992 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	strlen

	Description:
	This subroutine is a knock off of the |strlen(3c)| from the
	regular UNIX® system.

*******************************************************************************/

#include	<envstandards.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
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


/* external subroutines */

size_t strlen(cchar *s) noex {
	size_t		len = 0 ;
	if (s) {
	    while (*s++) len += 1 ;
	}
	return len ;
}
/* end subroutine (strlen) */



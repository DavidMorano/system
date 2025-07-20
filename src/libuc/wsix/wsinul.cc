/* wsinul SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get length of a wide-string */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	wsinul

	Description:
	Calculate the length of a wide-string.

	Synopsis:
	int wsinul(const wchar_t *wsp) noex

	Arguments:
	wsp	the source string that is to be copied

	Returns:
	>=0	the character pointer to the end of the destination
	<0	error (null-pointer)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| + {wchar_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
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

int wsinul(const wchar_t *wsp) noex {
	int	i = -1 ; /* return-value */
	if (wsp) {
	    for (i = 0 ; wsp[i] ; i += 1) ;
	}
	return i ;
}
/* end subroutine (wsinul) */



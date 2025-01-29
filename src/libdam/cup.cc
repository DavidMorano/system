/* cup SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* subroutine to convert a counted string to upper case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cup

	Description:
	This file contains many of the string manipulation subroutines
	used in other modules.

	Synopsis:
	char *cup(int len,cchar *src,char *dst) noex

	Returns:
	-		pointer to the end of filled-in destination buffer

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<toxc.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"cup.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

char *cup(int len,cchar *src,char *dst) noex {
	for (int i = 0 ; i < len ; i += 1) {
	    *dst++ = touc(*src++) ;
	} /* end for */
	*dst = '\0' ;
	return dst ;
}
/* end subroutine (cup) */



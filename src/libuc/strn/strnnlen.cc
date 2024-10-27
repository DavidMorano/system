/* strnnlen SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* calculate the length of a counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strnnlen

	Description:
	This subroutine computes the length of a supplied string
	but will not examine the string for more than a specified
	length.

	Synopsis:
	int strnnlen(cchar *sp,int sl,int max) noex

	Arguments:
	sp	string to be examined
	sl	length of string (or -1)
	nax	maximum length of string to be examined

	Returns:
	len	mimimum of length of string or MIN(slen,max)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- for |strnlen(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"strn.h"


/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int strnnlen(cchar *sp,int sl,int mlen) noex {
	int		len = 0 ;
	if (sp && (sl != 0) && (mlen != 0)) {
	    if (sl >= 0) {
	        if (mlen >= 0) sl = min(sl,mlen) ;
	    } else {
	        sl = mlen ;
	    }
	    len = strnlen(sp,sl) ;
	} /* end if */
	return len ;
}
/* end subroutine (strnnlen) */



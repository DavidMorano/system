/* snwcpyer SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* special (excellent) string-copy type of subroutine! */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snwcpy

	Description:
	Ths subroutine is similar to |sncpy1(3uc)| but it takes a
	counted string for the source rather than only a NUL-terminated
	string.  In actual fact, this subroutine is semantically
	identical to |sncpy1w(3uc)|.

	Synopsis:
	int snwcpy(char *dp,int dl,cchar *sp,int sl) noex

	Arguments:
	dp		destination string buffer
	dl		destination string buffer length
	sp		source string
	sl		source string length

	Returns:
	>=0		number of bytes in result
	<0		error (system-return)

	Notes:
	This subroutine just calls either the |sncpy1(3uc)| or the
	|strwcpy(3uc)| subroutine based on the arguments.  The
	advantage of this subroutine over the others is that the
	logic needed to figure out just what subroutine to call is
	coded into this subroutine already.  We try to use the most
	efficient string-copy operation that we can given the passed
	arguments, while also tracking whether a buffer overflow
	could occur.

	See-also:
	snwcpy(3uc),
	snwcpylatin(3uc), 
	snwcpyopaque(3uc), 
	snwcpycompact(3uc), 
	snwcpyclean(3uc), 
	snwcpyhyphen(3uc), 
	snwcpylc(3uc),
	snwcpyuc(3uc),
	snwcpyfc(3uc),

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<sncpyx.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"snwcpy.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int snwcpy(char *dp,int dl,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (dp && sp) {
	    if (dl >= 0) {
	        if (sl >= 0) {
	            if (sl > dl) {
	                rs = sncpy1(dp,dl,sp) ;
	            } else {
	                rs = intconv(strwcpy(dp,sp,sl) - dp) ;
		    }
	        } else {
	            rs = sncpy1(dp,dl,sp) ;
	        }
	    } else {
	        rs = intconv(strwcpy(dp,sp,sl) - dp) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (snwcpy) */



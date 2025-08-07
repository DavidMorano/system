/* mnwcpy SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* special (excellent) memory-copy type of subroutine! */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	mnwcpy

	Description:
	Ths subroutine is a cross between |sncpy1(3dam)| and
	|memcpy(3c)| but it takes a counted string for the source
	rather than only a NUL-terminated string.

	Synopsis:
	int mnwcpy(char *dp,int dl,cchar *sp,int sl) noex

	Arguments:
	dp		destination string buffer
	dl		destination string buffer length
	sp		source string
	sl		source string length

	Returns:
	>=0		number of bytes in result
	<0		error (system-return)

	Notes:
	This subroutine just calls either |memcpy(3c)|, |sncpy1(3dam)|,
	or |strwcpy(3dam)| based on the arguments.  The advantage
	of this subroutine over the others is that the logic needed
	to figure out just what subroutine to call is coded into
	this subroutine already.  We try to use the most efficient
	string-copy operation that we can given the passed arguments,
	while also tracking whether a buffer overflow could occur.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |memcpy(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<sncpyx.h>		/* |snwcpy1(3uc)| */
#include	<strwcpy.h>		/* |strwcpy(3uc)| */
#include	<localmisc.h>

#include	"mnwcpy.h"


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

int mnwcpy(char *dp,int dl,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ; /* return-value */
	if (dp && sp) {
	    if (dl >= 0) {
	        if (sl >= 0) {
	            if (sl <= dl) {
		        memcpy(dp,sp,sl) ;
		        rs = sl ;
	            } else {
	                rs = SR_OVERFLOW ;
		    }
	        } else {
	            rs = sncpy1(dp,dl,sp) ;
	        }
	    } else {
	        if (sl >= 0) {
		    memcpy(dp,sp,sl) ;
		    rs = sl ;
	        } else {
	            rs = intconv(strwcpy(dp,sp,-1) - dp) ;
	        }
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mnwcpy) */



/* usupport_strwcpy SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
	This was first written to give a little bit to UNIX® what
	we have in our own circuit-pack OSes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strwcpy

	Description:
	This subroutine sleeps for some number of milliseconds.

	Synopsis:
	char *strwcpy(char *dp,cchar *sp,int sl = -1) noex

	Arguments:

	Returns:

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |stpcpy(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<localmisc.h>

#include	"usupport_strwcpy.h"


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

namespace libu {
    char *strwcpy(char *dp,cchar *sp,int sl) noex {
    	if (dp && sp) ylikely {
	    if (sl >= 0) {
	        while (sl-- && *sp) *dp++ = *sp++ ;
	        *dp = '\0' ;
	    } else {
	        dp = stpcpy(dp,sp) ;
	    } /* end if */
	} else {
	    dp = nullptr ;
	} /* end if (non-null) */
	return dp ;
    }
} /* end namespace (libu) */



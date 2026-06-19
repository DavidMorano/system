/* strwcpyx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* copy a counted c-string to a destination buffer */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	strwcpy

	Description:
	This provides come counted-string functions.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |stpcpy(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"strwcpyx.h"


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
} /* end subroutine (strwcpy) */



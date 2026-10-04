/* usys_stpcpy SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* concatenate strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	usys_stpcpy
	stpcpy{x}

	Description:
	This subroutine concatenates c-strings into a single resulting
	destination c-string.  It will not overflow the destiantion
	character buffer length.¹  1. The result is always NUL terminated
	(even beyond the destination character buffer length if
	necessary).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |stpcpy(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */

#include	"usys_stpcpy.h"


/* local defines */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

char *stpcpy1(char *dp,cc *s1) noex {
	return usys_stpcpy(dp,1,s1) ;
} /* end subroutine (stpcpy1) */

char *stpcpy2(char *dp,cc *s1,cc *s2) noex {
	return usys_stpcpy(dp,2,s1,s2) ;
} /* end subroutine (stpcpy2) */

char *stpcpy3(char *dp,cc *s1,cc *s2,cc *s3) noex {
	return usys_stpcpy(dp,3,s1,s2,s3) ;
} /* end subroutine (stpcpy3) */

char *stpcpy4(char *dp,cc *s1,cc *s2,cc *s3,cc *s4) noex {
	return usys_stpcpy(dp,4,s1,s2,s3,s4) ;
} /* end subroutine (stpcpy4) */

char *stpcpy5(char *dp,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5) noex {
	return usys_stpcpy(dp,5,s1,s2,s3,s4,s5) ;
} /* end subroutine (stpcpy5) */

char *stpcpy6(char *dp,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,cc *s6) noex {
	return usys_stpcpy(dp,6,s1,s2,s3,s4,s5,s6) ;
} /* end subroutine (stpcpy6) */

char *usys_stpcpy(char *dp,int n,...) noex {
	va_list		ap ;
	if (dp) ylikely {
	    va_begin(ap,n) ;
	    for (int i = 0 ; i < n ; i += 1) {
	        cchar	*sp = (cchar *) va_arg(ap,char *) ;
		dp = stpcpy(dp,sp) ;
	    } /* end for */
	    va_end(ap) ;
	    *dp = '\0' ;
	} /* end if (non-null) */
	return dp ;
} /* end subroutine (usys_stpcpy) */



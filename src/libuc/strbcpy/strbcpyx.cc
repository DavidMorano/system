/* strbcpyx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* concatenate strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

	= 1999-12-03, David A­D­ Morano
	This was updated to use |strlcpy(3c)| when it was rumored
	to be coming as a new standard.  We are currently using our
	own implementation of that, but when it is supported by
	vendors this will all seemlessly transistion to using the
	vendor version.

	= 2011-12-09, David A­D­ Morano
	I got rid of the |strlcpy(3c)| usage.  It was never really
	needed anyway.  The code is certainly cleaner without it.
	And I do not really think it is a whole lot slower either
	since the various string lengths used are usually fairly
	small.  Other subroutines have gotten rid of |strlcpy(3c)|
	also without any complaints.

*/

/* Copyright © 1998,1999,2011 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	strbcpyx
	strbcpy{x}

	Description:
	This subroutine concatenates c-strings into a single resulting
	destination c-string.  It will not overflow the destiantion
	character buffer length.¹  1. The result is always NUL terminated
	(even beyond the destination character buffer length if
	necessary).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */

#include	"strbcpyx.h"


/* local defines */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

char *strbcpy1(char *dp,int dl,cc *s1) noex {
	return strbcpyx(dp,dl,1,s1) ;
} /* end subroutine (strbcpy1) */

char *strbcpy2(char *dp,int dl,cc *s1,cc *s2) noex {
	return strbcpyx(dp,dl,2,s1,s2) ;
} /* end subroutine (strbcpy2) */

char *strbcpy3(char *dp,int dl,cc *s1,cc *s2,cc *s3) noex {
	return strbcpyx(dp,dl,3,s1,s2,s3) ;
} /* end subroutine (strbcpy3) */

char *strbcpy4(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4) noex {
	return strbcpyx(dp,dl,4,s1,s2,s3,s4) ;
} /* end subroutine (strbcpy4) */

char *strbcpy5(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5) noex {
	return strbcpyx(dp,dl,5,s1,s2,s3,s4,s5) ;
} /* end subroutine (strbcpy5) */

char *strbcpy6(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,cc *s6) noex {
	return strbcpyx(dp,dl,6,s1,s2,s3,s4,s5,s6) ;
} /* end subroutine (strbcpy6) */

char *strbcpyx(char *dp,int dl,int n,...) noex {
	va_list		ap ;
	if (dp) ylikely {
	    va_begin(ap,n) ;
	    bool fbad = false ;
	    if (dl < 0) dl = INT_MAX ;
	    for (int i = 0 ; (dl > 0) && (i < n) ; i += 1) {
	        cchar	*sp = (cchar *) va_arg(ap,char *) ;
	        while ((dl > 0) && *sp) {
		    *dp++ = *sp++ ;
		    dl -= 1 ;
	        } /* end while */
		if ((fbad = ((dl == 0) && *sp))) break ;
	    } /* end for */
	    *dp = '\0' ;
	    if (fbad) dp = nullptr ;
	    va_end(ap) ;
	} /* end if (non-null) */
	return dp ;
} /* end subroutine (strbcpyx) */



/* strdcpyx SUPPORT */
/* lang=C++20 */

/* concatenate strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

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

	This subroutine concatenates strings into a single resulting string.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* for |INT_MAX| */
#include	<cstdarg>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>

#include	"strdcpyx.h"


/* local defines */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local strutures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

char *strdcpy1(char *dp,int dl,cc *s1) noex {
	return strdcpyx(dp,dl,1,s1) ;
}
/* end subroutine (strdcpy1) */

char *strdcpy2(char *dp,int dl,cc *s1,cc *s2) noex {
	return strdcpyx(dp,dl,2,s1,s2) ;
}
/* end subroutine (strdcpy2) */

char *strdcpy3(char *dp,int dl,cc *s1,cc *s2,cc *s3) noex {
	return strdcpyx(dp,dl,3,s1,s2,s3) ;
}
/* end subroutine (strdcpy3) */

char *strdcpy4(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4) noex {
	return strdcpyx(dp,dl,4,s1,s2,s3,s4) ;
}
/* end subroutine (strdcpy4) */

char *strdcpy5(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5) noex {
	return strdcpyx(dp,dl,5,s1,s2,s3,s4,s5) ;
}
/* end subroutine (strdcpy5) */

char *strdcpy6(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,cc *s6) noex {
	return strdcpyx(dp,dl,6,s1,s2,s3,s4,s5,s6) ;
}
/* end subroutine (strdcpy6) */

char *strdcpyx(char *dp,int dl,int n,...) noex {
	if (dp) {
	    va_list	ap ;
	    va_begin(ap,n) ;
	    if (dl < 0) dl = INT_MAX ;
	    for (int i = 0 ; (dl > 0) && (i < n) ; i += 1) {
	        cc	*sp = (cc *) va_arg(ap,char *) ;
	        while ((dl > 0) && (sp[0] != '\0')) {
		    *dp++ = *sp++ ;
		    dl -= 1 ;
	        }
	    } /* end for */
	    va_end(ap) ;
	    *dp = '\0' ;
	} /* end if (non-null) */
	return dp ;
}
/* end subroutine (strdcpyx) */



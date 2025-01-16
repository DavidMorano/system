/* strdcpyxw SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* concatenate strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

	= 1999-12-03, David A­D­ Morano
	This was updated to use |strlcpy(3c)| when it was rumored
	to be coming.  We are using our own version of |strlcpy(3c)|
	before it is provided by vendors.

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
	strdcpyx
	strdcpy{x}

	Description:
	This subroutine concatenates source c-strings into a single
	resulting destination character buffer c-string.  It will
	not overflow the destination character buffer.¹  The result is
	always NUL-terminated.  The last source c-string has an
	optional associated length to it.

	Notes:
	1. If necessary (in order to gurantee NUL-termination) a
	NUL will be written past the end (deterined by a supplied
	length) of the destination character buffer.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdarg>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

#include	"strdcpyxw.h"


/* local defines */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local strutures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

char *strdcpy1w(char *dp,int dl,cc *s1,int sl) noex {
	return strdcpyxw(dp,dl,1,s1,sl) ;
}
/* end subroutine (strdcpy1w) */

char *strdcpy2w(char *dp,int dl,cc *s1,cc *s2,int sl) noex {
	return strdcpyxw(dp,dl,2,s1,s2,sl) ;
}
/* end subroutine (strdcpy2w) */

char *strdcpy3w(char *dp,int dl,cc *s1,cc *s2,cc *s3,int sl) noex {
	return strdcpyxw(dp,dl,3,s1,s2,s3,sl) ;
}
/* end subroutine (strdcpy3w) */

char *strdcpy4w(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4,int sl) noex {
	return strdcpyxw(dp,dl,4,s1,s2,s3,s4,sl) ;
}
/* end subroutine (strdcpy4w) */

char *strdcpy5w(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4,
		cc *s5,int sl) noex {
	return strdcpyxw(dp,dl,5,s1,s2,s3,s4,s5,sl) ;
}
/* end subroutine (strdcpy5w) */

char *strdcpy6w(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4,
		cc *s5,cc *s6,int sl) noex {
	return strdcpyxw(dp,dl,6,s1,s2,s3,s4,s5,s6,sl) ;
}
/* end subroutine (strdcpy6w) */

char *strdcpyxw(char *dp,int dl,int n,...) noex {
	va_list		ap ;
	if (dp) {
	    va_begin(ap,n) ;
	    if (dl < 0) dl = INT_MAX ;
	    for (int i = 0 ; (dl > 0) && (i < n) ; i += 1) {
	        cchar	*sp = (cchar *) va_arg(ap,char *) ;
	        if (i == (n-1)) {
	            int	sl = (int) va_arg(ap,int) ;
	            while ((dl > 0) && sl && (sp[0] != '\0')) {
		        *dp++ = *sp++ ;
		        dl -= 1 ;
		        sl -= 1 ;
	            }
	        } else {
	            while ((dl > 0) && (sp[0] != '\0')) {
		        *dp++ = *sp++ ;
		        dl -= 1 ;
	            }
	        }
	    } /* end for */
	    *dp = '\0' ;
	    va_end(ap) ;
	} /* end if (non-null) */
	return dp ;
}
/* end subroutine (strdcpyxw) */



/* strwcpyx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* copy a counted source c-string to a destination buffer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	strwcpy{x}

	Description:
	These subroutines copy a source counted-string to a non-counted
	destination buffer.  If you need something that prevents
	overflow in a counted-buffer destination, look elsewhere.
	The destination buffer is always NUL-terminated.

	Synopsis:
	char *strwcpyx(char *dbuf,int n,cchar *sp ...,int sl) noex
	char *strwcpy{x}(char *dbuf,cchar *sp ...,int sl) noex

	Arguments:
	n		number of source c-strings
	{x}		one of: 1, 2, 3, 4, 5, 6
	dbuf		destination buffer (not counted)
	sp		source c-string pointer
	sl		source c-string length

	Returns:
	-		pointer to the character following the last copied

	Notes:
	The count of the last source c-string only applies to the last
	source c-string.  Any intermediate source c-strings must be 
	NUL-terminated (since no count applies to them).

	Note (2026-10-10):
	This subroutine |strwcpy(3u)| is quite famous now.  Just
	saying.  Ya, it took me all of these years to say it.  I
	do not remember when I first wrote this subroutine, but it
	must have been in the early 1990s or possibly the late
	1980s.  More power to this subroutine!

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

local inline char *srscpy(char *dp,cchar *sp,int sl) noex {
	while (sl-- && *sp) *dp++ = *sp++ ;
	return dp ;
} /* end subroutine (srscpy) */


/* local variables */


/* exported variables */


/* exported subroutines */

char *strwcpy(char *dp,cchar *sp,int sl) noex {
    	if (dp && sp) ylikely {
	    if (sl >= 0) {
		dp = srscpy(dp,sp,sl) ;
	        *dp = '\0' ;
	    } else {
	        dp = stpcpy(dp,sp) ;
	    } /* end if */
	} else {
	    dp = nullptr ;
	} /* end if (non-null) */
	return dp ;
} /* end subroutine (strwcpy) */

char *strwcpy1(char *dp,
		cc *s1,int sl) noex {
	return strwcpyx(dp,1,s1,sl) ;
} /* end subroutine (strwcpy1) */

char *strwcpy2(char *dp,
		cc *s1,cc *s2,int sl) noex {
	return strwcpyx(dp,2,s1,s2,sl) ;
} /* end subroutine (strwcpy2) */

char *strwcpy3(char *dp,
		cc *s1,cc *s2,cc *s3,int sl) noex {
	return strwcpyx(dp,3,s1,s2,s3,sl) ;
} /* end subroutine (strwcpy3) */

char *strwcpy4(char *dp,
		cc *s1,cc *s2,cc *s3,cc *s4,int sl) noex {
	return strwcpyx(dp,4,s1,s2,s3,s4,sl) ;
} /* end subroutine (strwcpy4) */

char *strwcpy5(char *dp,
		cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,int sl) noex {
	return strwcpyx(dp,5,s1,s2,s3,s4,s5,sl) ;
} /* end subroutine (strwcpy5) */

char *strwcpy6(char *dp,
		cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,cc *s6,int sl) noex {
	return strwcpyx(dp,6,s1,s2,s3,s4,s5,s6,sl) ;
} /* end subroutine (strwcpy6) */

char *strwcpyx(char *dp,int n,...) noex {
	va_list		ap ;
	if (dp) ylikely {
	    va_begin(ap,n) ;
	    if (n > 1) {
	        for (int i = 0 ; i < (n - 1) ; i += 1) {
	            ccp sp = (cchar *)	va_arg(ap,char *) ;
		    dp = stpcpy(dp,sp) ;
		} /* end for */
	    } /* end if */
	    if (n > 0) {
		{
	            ccp sp = (cchar *)	va_arg(ap,char *) ;
	            int	sl = (int)	va_arg(ap,int) ;
		    dp = srscpy(dp,sp,sl) ;
		} /* end */
	    } /* end for */
	    *dp = '\0' ;
	    va_end(ap) ;
	} /* end if (non-null) */
	return dp ;
} /* end subroutine (strwcpyx) */



/* strnwcpyx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* copy a counted c-string to a destination buffer w/ zero-fill */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	strnwcpy{x}

	Description:
	These subroutines copy a source counted-string (or not) to
	a counted-buffer destination.  There is no overflow of the
	destination buffer (any remaining source is just ignored).
	If you need something that prevents overflow in a
	counted-buffer destination, look elsewhere.

	Synopsis:
	char *strnwcpyx(char *dbuf,int dlen,int n,cchar *sp ...,int sl) noex
	char *strnwcpy{x}(char *dbuf,int dlen,cchar *sp ...,int sl) noex

	Arguments:
	dbuf		destination buffer pointer
	dlen		destination buffer length
	n		number of source c-strings
	{x}		one of: 1, 2, 3, 4, 5, 6
	sp		source c-string pointer
	sl		source c-string length

	Returns:
	-		pointer to the character following the last copied

	Notes:
	1. The count of the last source c-string only applies to
	the last source c-string.  Any intermediate source c-strings
	must be NUL-terminated (since no count applies to them).
	2. These subroutines, and especially focussing on the
	flagship subroutine |strnwcpy(3uc)|, may seem a bit strange,
	at least at first.  That is, why is something like this
	(reference the function signature) needed and where could
	it possibly be used constructively (solving some real
	problem)?  Well, somewhat surprisingly, the flagship
	subroutine |strnwcpy(3uc)| actually is very widely used in
	zezo-filling a variety of system-related buffers, especially
	in certain networking contexts.  That subroutine has been
	widely used (somewhat surprisingly) since at least the early
	1990s.  It may have been used before that but I forget about
	any prior uses.  On second thought, and subsequent thoughts,
	once it is realized that source c-strings can be counted
	themselves (meaning that their end is defined by a count
	rather than a NUL terminating character) it becomes easier
	to realize that something like |strnwcpy(3uc)| is actually
	both usedul and indeed needed.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<cstring>		/* CSTD |stpcpy(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"strnwcpyx.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local inline char *dstcpy(char *dp,int dl,cchar *sp,int sl) noex {
	while (dl-- && sl-- && *sp) {
	    *dp++ = *sp++ ;
	} /* end while */
	return dp ;
} /* en subroutine (dstcpy) */

local inline char *dstcpy(char *dp,int dl,cchar *sp) noex {
	while (dl-- && *sp) {
	    *dp++ = *sp++ ;
	} /* end while */
	return dp ;
} /* en subroutine (dstcpy) */


/* local variables */


/* exported variables */


/* exported subroutines */

char *strnwcpy(char *dp,int dl,cchar *sp,int sl) noex {
    	if (dp && sp) ylikely {
	    if (dl >= 0) {
		char *ep = (dp + dl) ;
		if (sl >= 0) {
		    dp = dstcpy(dp,dl,sp,sl) ;
		    dl = intconv(ep - dp) ;
		} else {
		    dp = dstcpy(dp,dl,sp) ;
		    dl = intconv(ep - dp) ;
		} /* end if */
	        if (dl > 0) {
	            memclear(dp,dl) ;
	        } /* end if (zero-fill) */
	    } else {
	        if (sl >= 0) {
	            while (sl-- && *sp) *dp++ = *sp++ ;
	            *dp = '\0' ;
	        } else {
	            dp = stpcpy(dp,sp) ;
	        } /* end if */
	    } /* end if */
	} else {
	    dp = nullptr ;
	} /* end if (non-null) */
	return dp ;
} /* end subroutine (strnwcpy) */

char *strnwcpy1(char *dp,int dl,
		cc *s1,int len) noex {
	return strnwcpyx(dp,dl,1,len,s1) ;
} /* end subroutine (strnwcpy1) */

char *strnwcpy2(char *dp,int dl,
		cc *s1,cc *s2,int len) noex {
	return strnwcpyx(dp,dl,2,len,s1,s2) ;
} /* end subroutine (strnwcpy2) */

char *strnwcpy3(char *dp,int dl,
		cc *s1,cc *s2,cc *s3,int len) noex {
	return strnwcpyx(dp,dl,3,len,s1,s2,s3) ;
} /* end subroutine (strnwcpy3) */

char *strnwcpy4(char *dp,int dl,
		cc *s1,cc *s2,cc *s3,cc *s4,int len) noex {
	return strnwcpyx(dp,dl,4,len,s1,s2,s3,s4) ;
} /* end subroutine (strnwcpy4) */

char *strnwcpy5(char *dp,int dl,
		cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,int len) noex {
	return strnwcpyx(dp,dl,5,len,s1,s2,s3,s4,s5) ;
} /* end subroutine (strnwcpy5) */

char *strnwcpy6(char *dp,int dl,
		cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,cc *s6,int len) noex {
	return strnwcpyx(dp,dl,6,len,s1,s2,s3,s4,s5,s6) ;
} /* end subroutine (strnwcpy6) */

local char *strnwcpyx_pos(char *dp,int dl,int n,int len,va_list ap) noex {
	char *rp ;
	if (n > 1) {
	    for (int i = 0 ; (dl > 0) && (i < (n - 1)) ; i += 1) {
	        ccharp sp = (cchar *) va_arg(ap,char *) ;
		if (! sp) break ;
		rp = dstcpy(dp,dl,sp) ;
		dl -= intconv(rp - dp) ;
		dp = rp ;
	    } /* end for */
	} /* end if */
	if (n > 0) {
	    if (dl > 0) {
	        ccharp sp = (cchar *) va_arg(ap,char *) ;
		if (sp) {
		    rp = dstcpy(dp,dl,sp,len) ;
		    dl -= intconv(rp - dp) ;
		    dp = rp ;
		}
	    } /* end block */
	} /* end for */
	if (dl > 0) {
	    memclear(dp,dl) ;
	} /* end if (zero-fill) */
	return dp ;
} /* end subroutine (strnwcpyx_pos) */

local char *strnwcpyx_neg(char *dp,int dl,int n,int len,va_list ap) noex {
	(void) dl ;
	if (n > 1) {
	    for (int i = 0 ; i < (n - 1) ; i += 1) {
	        ccharp sp = (cchar *) va_arg(ap,char *) ;
		if (! sp) break ;
		dp = stpcpy(dp,sp) ;
	    } /* end for */
	} /* end if */
	if (n > 0) {
	    {
	        ccharp sp = (cchar *) va_arg(ap,char *) ;
		if (sp) {
		    dp = dstcpy(dp,len,sp) ;
		}
	    }
	} /* end for */
	*dp = '\0' ;
	return dp ;
} /* end subroutine (strnwcpyx_neg) */

char *strnwcpyx(char *dp,int dl,int n,int len,...) noex {
	va_list		ap ;
	if (dp) ylikely {
	    va_begin(ap,len) ;
	    if (dl >= 0) {
		dp = strnwcpyx_pos(dp,dl,n,len,ap) ;
	    } else {
		dp = strnwcpyx_neg(dp,dl,n,len,ap) ;
	    } /* end if */
	    va_end(ap) ;
	} /* end if (non-null) */
	return dp ;
} /* end subroutine (strnwcpyx) */



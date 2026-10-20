/* snwcpyx SUPPORT */
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

	Group:
	snwcpy{x}

	Description:
	This subroutine is similar to |sncpy1(3uc)| but it takes a
	counted string for the source rather than only a NUL-terminated
	string.  In actual fact, this subroutine is semantically
	identical to |sncpy1w(3uc)|.

	Synopsis:
	int snwcpy(char *dp,int dl,cchar *sp,int sl) noex
	int snwcpy{x}(char *dp,int dl,cchar *sp ...,int sl) noex

	Arguments:
	{x}		one of: 1, 2, 3 ,4, 5, 6
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

	Notes:
	I am using the new (rumored to be coming as a standard)
	subroutine |strlcpy(3c)|.  I wrote my own version of this
	until it gets into the standard libraries.  It might be in
	someone's standard library already, but I do not have that
	in any of the operating systems I am working with.  And no,
	I do not like the function signature of that new interface
	(API).  I do not like it, but if it is going to become a
	new standard, it might get optimized for speed (like
	writting in hand-coded assembly language).  So that is
	pretty much the only reason I would use that interface
	(which I do not like).

	See-also:
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
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<strbcpy.h>		/* LIBU */
#include	<sncpyx.h>		/* LIBUC |sncpy(3uc)| */
#include	<strwcpy.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"snwcpyx.h"


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
	if (dp && sp) ylikely {
	    if (dl >= 0) {
	        if (sl >= 0) {
	            if (sl > dl) {
	                rs = sncpy(dp,dl,sp) ;
	            } else {
	                rs = intconv(strwcpy(dp,sp,sl) - dp) ;
		    } /* end */
	        } else {
	            rs = sncpy(dp,dl,sp) ;
	        } /* end */
	    } else {
	        rs = intconv(strwcpy(dp,sp,sl) - dp) ;
	    } /* end */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (snwcpy) */

int snwcpy1(char *dp,int dl,
		cc *s1,int sl) noex {
	return snwcpyx(dp,dl,1,s1,sl) ;
} /* end subroutine (snwcpy1) */

int snwcpy2(char *dp,int dl,
		cc *s1,cc *s2,int sl) noex {
	return snwcpyx(dp,dl,2,s1,s2,sl) ;
} /* end subroutine (snwcpy2) */

int snwcpy3(char *dp,int dl,
		cc *s1,cc *s2,cc *s3,int sl) noex {
	return snwcpyx(dp,dl,3,s1,s2,s3,sl) ;
} /* end subroutine (snwcpy3) */

int snwcpy4(char *dp,int dl,
		cc *s1,cc *s2,cc *s3,cc *s4,int sl) noex {
	return snwcpyx(dp,dl,4,s1,s2,s3,s4,sl) ;
} /* end subroutine (snwcpy4) */

int snwcpy5(char *dp,int dl,
		cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,int sl) noex {
	return snwcpyx(dp,dl,5,s1,s2,s3,s4,s5,sl) ;
} /* end subroutine (snwcpy5) */

int snwcpy6(char *dp,int dl,
		cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,cc *s6,int sl) noex {
	return snwcpyx(dp,dl,6,s1,s2,s3,s4,s5,s6,sl) ;
} /* end subroutine (snwcpy6) */

local int snwcpyx_pos(char *dp,int dl,int na,va_list ap) noex {
	int		rs = SR_OVERFLOW ;
	char	*bp = dp ;
	for (int i = 0 ; dp && (dl > 0) && (i < (na - 1)) ; i += 1) {
	    ccp  sp = (cchar *) va_arg(ap,char *) ;
	    ccp  cp = dp ;
	    if (! sp) break ;
	    dp = strbcpy(dp,dl,sp) ;
	    dl -= intconv(dp - cp) ;
	} /* end for */
	if (dp && (na > 0)) ylikely {
	    ccp  sp = (cchar *) va_arg(ap,char *) ;
	    cint sl = (int) va_arg(ap,int) ;
	    if (sp) dp = strbcpy(dp,dl,sp,sl) ;
	} /* end for */
	if (dp) {
	    rs = intconv(bp - dp) ;
	} /* end if */
	return rs ;
} /* end subroutine (snwcpyx_pos) */

local int snwcpyx_neg(char *dp,int dl,int na,va_list ap) noex {
	int		rs = SR_OK ;
	(void) dl ;
	char	*bp = dp ;
	for (int i = 0 ; i < (na - 1) ; i += 1) {
	    ccp  sp = (cchar *) va_arg(ap,char *) ;
	    if (! sp) break ;
	    dp = stpcpy(dp,sp) ;
	} /* end for */
	if (na > 0) ylikely {
	    ccp  sp = (cchar *) va_arg(ap,char *) ;
	    cint sl = (int) va_arg(ap,int) ;
	    if (sp) dp = strwcpy(dp,sp,sl) ;
	} /* end if */
	rs = intconv(bp - dp) ;
	return rs ;
} /* end subroutine (snwcpyx_neg) */

int snwcpyx(char *dp,int dl,int na,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	if (dp) ylikely {
	    va_begin(ap,na) ;
	    *dp = eos ;
	    if (dl >= 0) {
		rs = snwcpyx_pos(dp,dl,na,ap) ;
	    } else {
		rs = snwcpyx_neg(dp,dl,na,ap) ;
	    } /* end if */
	    va_end(ap) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (snwcpyx) */


/* local subroutines */



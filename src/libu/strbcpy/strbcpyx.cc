/* strbcpyx SUPPORT (String-Buffer-Copy) */
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
	vendors this will all seemlessly transition to using the
	vendor version.

	= 2011-12-09, David A­D­ Morano
	I got rid of the usage of the subroutine |strlcpy(3c)| in
	the subroutine |strbcpyx()| below.

*/

/* Copyright © 1998,1999,2011 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	strbcpyx
	strbcpy{x}

	Description:
	This subroutine concatenates c-strings into a single resulting
	destination c-string.  It will not overflow the destiantion
	character buffer length.  The result is always NUL terminated
	(even beyond the destination character buffer length if
	necessary).

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

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |stpcpy(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

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

char *strbcpy(char *dp,int dl,cchar *sp) noex {
    	return usys::strbcpy(dp,dl,sp) ;
} /* end subroutine (strbcpy) */

char *strbcpy1(char *dp,int dl,
		cc *s1) noex {
	return strbcpyx(dp,dl,1,s1) ;
} /* end subroutine (strbcpy1) */

char *strbcpy2(char *dp,int dl,
		cc *s1,cc *s2) noex {
	return strbcpyx(dp,dl,2,s1,s2) ;
} /* end subroutine (strbcpy2) */

char *strbcpy3(char *dp,int dl,
		cc *s1,cc *s2,cc *s3) noex {
	return strbcpyx(dp,dl,3,s1,s2,s3) ;
} /* end subroutine (strbcpy3) */

char *strbcpy4(char *dp,int dl,
		cc *s1,cc *s2,cc *s3,cc *s4) noex {
	return strbcpyx(dp,dl,4,s1,s2,s3,s4) ;
} /* end subroutine (strbcpy4) */

char *strbcpy5(char *dp,int dl,
		cc *s1,cc *s2,cc *s3,cc *s4,cc *s5) noex {
	return strbcpyx(dp,dl,5,s1,s2,s3,s4,s5) ;
} /* end subroutine (strbcpy5) */

char *strbcpy6(char *dp,int dl,
		cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,cc *s6) noex {
	return strbcpyx(dp,dl,6,s1,s2,s3,s4,s5,s6) ;
} /* end subroutine (strbcpy6) */

char *strbcpyx_pos(char *dp,int dl,int na,va_list ap) noex {
	bool fbad = false ;
	for (int i = 0 ; (dl > 0) && (i < na) ; i += 1) {
	    ccharp sp = (cchar *) va_arg(ap,char *) ;
	    if (! sp) break ;
	    while (dl && *sp) {
		*dp++ = *sp++ ;
		dl -= 1 ;
	    } /* end while */
	    if ((fbad = ((dl == 0) && *sp))) break ;
	} /* end for */
	*dp = eos ;
	if (fbad) dp = nullptr ;
	return dp ;
} /* end subroutine (strbcpyx_pos) */

char *strbcpyx_neg(char *dp,int dl,int na,va_list ap) noex {
    	(void) dl ;
	for (int i = 0 ; i < na ; i += 1) {
	    ccharp sp = (cchar *) va_arg(ap,char *) ;
	    if (! sp) break ;
	    dp = stpcpy(dp,sp) ;
	} /* end for */
	return dp ;
} /* end subroutine (strbcpyx_neg) */

char *strbcpyx(char *dp,int dl,int na,...) noex {
	va_list		ap ;
	if (dp) ylikely {
	    va_begin(ap,na) ;
	    if (dl >= 0) {
		dp = strbcpyx_pos(dp,dl,na,ap) ;
	    } else {
		dp = strbcpyx_neg(dp,dl,na,ap) ;
	    } /* end if */
	    va_end(ap) ;
	} /* end if (non-null) */
	return dp ;
} /* end subroutine (strbcpyx) */



/* sncpyxw SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* concatenate strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

	= 1999-12-03, David A­D­ Morano
	This was updated to use |strlcpy(3c)| when it was rumored
	to be coming as a new standard.  We are currently using our
	own implementation of that, but when it is supported by
	vendors this will all seemlessly transition to using the
	vendor version.

	= 2026-10-14, David A­D­ Morano
	I removed the use of the subroutine |strlcpy(3c)|.  It just
	was the wrong choice for what I want these subroutines to
	do.  It was replaced w/ |strbcpy(3usys)|.

*/

/* Copyright © 1998,1999,2026 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	sncpy{x}w

	Description:
	This subroutine concatenates strings into a single resulting
	string.

	Notes:
	These sorts of subroutines (|sncpyw(3uc)|, |sncpy(3uc)|,
	etc) are quite carefully crafted in order to avoid buffer
	overflow and segfaults as well as leaving the destination
	buffer NUL-terminated.  Please observe and be careful with
	any possible modifications.

	Notes: (obsoleted 2026-10-14, see revision above)
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
	snxtilook(3uc)
	sninetaddr(3uc)
	snsigabbr(3uc)
	snabbr(3uc)
	snshellunder(3uc)
	snfilemode(3uc)
	sntid(3uc)
	snerrabbr(3uc)
	snrealname(3uc)
	snloadavg(3uc)
	snkeyval(3uc)
	snwvprintf(3uc)
	snwprintf(3uc)
	snkeyval(3uc)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD |va_list(3c)| */
#include	<cstring>		/* CSTD |strlcpy(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU |strbcpy(3usys)| */
#include	<localmisc.h>		/* LIBU */

#include	"sncpyxw.h"


/* local typedefs */


/* imported namespaces */

using usys::strbcpy ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external varaibles */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sncpy1w(char *dp,int dl,
		cc *s1,int sl) noex {
	return sncpyxw(dp,dl,1,s1,sl) ;
} /* end subroutine (sncpy1w) */

int sncpy2w(char *dp,int dl,
		cc *s1,cc *s2,int sl) noex {
	return sncpyxw(dp,dl,2,s1,s2,sl) ;
} /* end subroutine (sncpy2w) */

int sncpy3w(char *dp,int dl,
		cc *s1,cc *s2,cc *s3,int sl) noex {
	return sncpyxw(dp,dl,3,s1,s2,s3,sl) ;
} /* end subroutine (sncpy3w) */

int sncpy4w(char *dp,int dl,
		cc *s1,cc *s2,cc *s3,cc *s4,int sl) noex {
	return sncpyxw(dp,dl,4,s1,s2,s3,s4,sl) ;
} /* end subroutine (sncpy4w) */

int sncpy5w(char *dp,int dl,
		cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,int sl) noex {
	return sncpyxw(dp,dl,5,s1,s2,s3,s4,s5,sl) ;
} /* end subroutine (sncpy5w) */

int sncpy6w(char *dp,int dl,
		cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,cc *s6,int sl) noex {
	return sncpyxw(dp,dl,6,s1,s2,s3,s4,s5,s6,sl) ;
} /* end subroutine (sncpy6w) */

int sncpyxw(char *dp,int dl,int n,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	if (dl < 0) dl = INT_MAX ;
	if (dp) ylikely {
	    va_begin(ap,n) ;
	    char	*bp = dp ;
	    rs = SR_OVERFLOW ;
	    dp[0] = '\0' ;
	    for (int i = 0 ; dp && (i < (n - 1)) ; i += 1) {
	        ccp  sp = (cchar *) va_arg(ap,char *) ;
		ccp  cp = dp ;
		if (! sp) break ;
	        dp = strbcpy(dp,dl,sp) ;
		dl -= intconv(dp - cp) ;
	    } /* end for */
	    if (dp && (n > 0) && dl) {
	        ccp  sp = (cchar *) va_arg(ap,char *) ;
	        cint sl = (int) va_arg(ap,int) ;
		if (sp) dp = strbcpy(dp,dl,sp,sl) ;
	    } /* end if */
	    if (dp) {
		rs = intconv(dp - bp) ;
	    } /* end */
	    va_end(ap) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (sncpyxw) */



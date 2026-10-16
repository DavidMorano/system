/* sncpyx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* concatenate c-strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

	= 1999-12-03, David A­D­ Morano
	This was updated to use |strlcpy(3c)| when it was rumored
	to be coming as a new standard.  We are currently using our
	own implementation of that, but when it is supported by
	vendors this will all seemlessly (pretty much) transition
	to using the vendor version. My |strlcpy(3u)| uses an
	|int| for the buffer size while the new |strlcpy(3c)| is
	suppoed to use |size_t|.

	= 2026-10-14, David A­D­ Morano
	I removed the use of the subroutine |strlcpy(3c)|.  It just
	was the wrong choice for what I want these subroutines to
	do.  It was replaced w/ |strbcpy(3usys)|.

*/

/* Copyright © 1998,1999,2026 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	sncpy{x}

	Description:
	These subroutines concatenate source c-strings into a single
	resulting counted destination character buffer (forming
	a resulting c-string).

	Synopsis:
	int sncpy{x}(charp dbuf,int dlen,cchar *sp ...) noex

	Arguments:
	{x}		one of: 1, 2, 3, 4, 5, 6
	dbuf		destiantion character buffer pointer
	dlen		destiantion character buffer length
	sp		source c-sring (NUL-terminated)

	Returns:
	>=0		number of bytes copied into the destination buffer
	<0		error (system-return)

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

	Observation 2015-09-29, David A­D­ Morano:
	Just some musing here: was the choice of changing to use
	|strlcpy(3c)| really such a good idea after all?  Just
	wondering.  Yes, we used it now for tons of years, but was
	it really faster than what we had before?  This goes diddo
	for the other places which changed the code to use
	|strlcpy(3c)|.

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
#include	<cstring>		/* LIBUC |stpcpy(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU |strbcpy(3usys)| */
#include	<localmisc.h>		/* LIBU */

#include	"sncpyx.h"


/* imported namespaces */

using usys::strbcpy ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sncpy(char *dp,int dl,cchar *sp) noex {
	int		rs = SR_FAULT ;
	if (dp && sp) ylikely {
	    if (dl >= 0) {
	        if (char *rp = strbcpy(dp,dl,sp)) {
		    rs = intconv(rp - dp) ;
		} else {
		    rs = SR_OVERFLOW ;
		} /* end */
	    } else {
	        rs = intconv(stpcpy(dp,sp) - dp) ;
	    } /* end */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (snwcpy) */

int sncpy1(char *dp,int dl,cc *s1) noex {
	return sncpyx(dp,dl,1,s1) ;
} /* end subroutine (sncpy1) */

int sncpy2(char *dp,int dl,cc *s1,cc *s2) noex {
	return sncpyx(dp,dl,2,s1,s2) ;
} /* end subroutine (sncpy2) */

int sncpy3(char *dp,int dl,cc *s1,cc *s2,cc *s3) noex {
	return sncpyx(dp,dl,3,s1,s2,s3) ;
} /* end subroutine (sncpy3) */

int sncpy4(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4) noex {
	return sncpyx(dp,dl,4,s1,s2,s3,s4) ;
} /* end subroutine (sncpy4) */

int sncpy5(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5) noex {
	return sncpyx(dp,dl,5,s1,s2,s3,s4,s5) ;
} /* end subroutine (sncpy5) */

int sncpy6(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,cc *s6) noex {
	return sncpyx(dp,dl,6,s1,s2,s3,s4,s5,s6) ;
} /* end subroutine (sncpy6) */

local int sncpyx_pos(char *dp,int dl,int n,va_list ap) noex {
    	int		rs = SR_OVERFLOW ;
	char *bp = dp ;
	for (int i = 0 ; dp && (i < n) ; i += 1) {
	    ccp sp = (cc *) va_arg(ap,cc *) ;
	    ccp cp = dp ;
	    if (! sp) break ;
	    dp = strbcpy(dp,dl,sp) ;
	    if (dp == nullptr) break ;
	    dl -= intconv(dp - cp) ;
	} /* end for */
	if (dp) {
	    rs = intconv(dp - bp) ;
	} /* end if */
	return rs ;
} /* end subroutine (sncpyx_pos) */

local int sncpyx_neg(char *dp,int dl,int n,va_list ap) noex {
    	int		rs = SR_OVERFLOW ;
	char *bp = dp ;
	(void) dl ;
	for (int i = 0 ; dp && (i < n) ; i += 1) {
	    ccp sp = (cc *) va_arg(ap,cc *) ;
	    if (! sp) break ;
	    dp = stpcpy(dp,sp) ;
	} /* end for */
	if (dp) {
	    rs = intconv(dp - bp) ;
	} /* end if */
	return rs ;
} /* end subroutine (sncpyx_neg) */

int sncpyx(char *dp,int dl,int n,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	if (dp) ylikely {
	    va_begin(ap,n) ;
	    if (dl >= 0) {
		rs = sncpyx_pos(dp,dl,n,ap) ;
	    } else {
		rs = sncpyx_neg(dp,dl,n,ap) ;
	    } /* end if */
	    va_end(ap) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (sncpyx) */



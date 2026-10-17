/* modsncpy MODULE (implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
	This was first written to give a little bit to UNIX® what
	we have in our own circuit-pack OSes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	sncpy{x}

	Description:
	These are the famous counted-destination-buffer copy
	subroutines.

	Synopsis:
	int sncpy{x}(char *dp,int dl,char *s1, ...) noex

	Arguments:
	dp		destination buffer pointer
	dl		destination buffer length
	s1		source string (first)

	Returns:
	>=0		amount of data returned
	<0		error (system-return)

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdarg>		/* CSTD |va_list(3c)| + |va_arg(3c)| */
#include	<cstring>		/* CSTD |strbcpy(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usys.h>		/* LIBU |strbcpy(3usys)| */
#include	<localmisc.h>		/* LIBU */

module modsncpy ;


/* local defines */


/* imported namespaces */


/* local typedefs */

using usys::strbcpy ;			/* subroutine */


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


/* local subroutines */



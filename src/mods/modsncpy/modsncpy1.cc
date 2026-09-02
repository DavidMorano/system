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
#include	<cstring>		/* CSTD |strlcpy(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

module modsncpy ;


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

int sncpyx(char *dp,int dl,int n,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (dl < 0) dl = (INT_MAX - 1) ;
	if (dp) {
	    char	*bp = dp ;
	    size_t	rlen = (dl + 1) ;
	    va_begin(ap,n) ;
	    rs = SR_OK ;
	    dp[0] = '\0' ;
	    for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
	        cc	*sp = (cc *) va_arg(ap,cc *) ;
	        if (size_t ml ; (ml = strlcpy(bp,sp,rlen)) >= rlen) {
	            rs = SR_OVERFLOW ;
		    break ;
	        } else {
	            bp += ml ;
	            rlen -= ml ;
		}
	    } /* end for */
	    rl = intconv(bp - dp) ;
	    va_end(ap) ;
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (sncpyx) */



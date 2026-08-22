/* xpow SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* calculate integer-power */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-03-01, David A­D­ Morano
	This code was originally written.  

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ipow

	Description:
	This subroutine calculates and returns an integer power for
	a given base.

	Synopsis:
	int ipow(int b,int p) noex

	Arguments:
	b	base
	p	power

	Returns:
	-	result (b**p)

	Notes: 
	Only defined for positive exponents.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |CHAR_BIT| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<bit>			/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"pow.h"
#include	"xpow.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

constexpr int	b10tab[] = {
	1, 10, 100, 1000, 10000, 100000, 1000000,
	10000000, 100000000, 1000000000
} ; /* end array */


/* forward references */

local inline int ipow10(int n) noex {
	return b10tab[n] ;
} /* end */

template<typename T> local T xpowraw(T b,int n) noex {
    	T	r = 1 ;
	if (n == 1) {
	    r = b ;
	} else if (n == 2) { /* common case */
	    r = b * b ;
	} else if (n > 2) {
	    con T t = xpowraw(b,(n / 2)) ;
	    if ((n & 1) == 0) {
		r = (t * t) ;
	    } else {
		r = b * (t * t) ;
	    }
	} /* end if */
	return r ;
} /* end subroutine (xpowraw) */

template<typename T> local T xpowten(T b,int n) noex {
	int		r = 0 ;
	if (n < 9) {
	    r = b10tab[n] ;
	} else if (szof(T) > szof(int)) {
	    r = xpowraw(b,n) ;
	} /* end if */
	return r ;
} /* end subroutine (ipowten) */

template<typename T> local T xpow(T b,int n) noex {
	T	r = 1 ;
	if (b == 10) {
	    r = ipow10(n) ;
	} else if (b == 2) {
	    cint nb = (szof(T) * CHAR_BIT) ;
	    if (n < nb) {
	        r = (1 << n) ;
	    } else {
		r = 0 ;
	    }
	} else {
	    r = xpowraw(b,n) ;
	} /* end if (base-specialization) */
	return r ;
} /* end subroutine (zpow) */


/* local variables */


/* exported variables */


/* exported subroutines */

int		ipow(int b,int n) noex {
    	return xpow(b,n) ;
} /* end subroutine (ipow) */

long 		lpow(long b,int n) noex {
    	return xpow(b,n) ;
} /* end subroutine (lpow) */

longlong	llpow(longlong b,int n) noex {
    	return xpow(b,n) ;
} /* end subroutine (llpow) */


/* local subroutines */



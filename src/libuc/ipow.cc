/* ipow SUPPORT */
/* lang=C++20 */

/* calculate integer-power */
/* version %I% last-modified %G% */

#define	CF_DYNAMIC	1		/* dynamic programming */

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
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>		/* 'LONG' type */

#include	"pow.h"


/* local defines */

#ifndef	CF_DYNAMIC
#define	CF_DYNAMIC	1		/* dynamic programming */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	ipow10(int) noex ;


/* local variables */

constexpr bool	f_dynamic = CF_DYNAMIC ;

constexpr int	b20tab[] = {
	1, 10, 100, 1000, 10000, 100000, 1000000,
	10000000, 100000000, 1000000000
} ;


/* exported variables */


/* exported subroutines */

int ipow(int b,int n) noex {
	int		r = 1 ;
	if (b == 10) {
	    r = ipow10(n) ;
	} else if (b == 2) {
	    if (b < 32) {
	        r = (1 << n) ;
	    } else {
		r = 0 ;
	    }
	} else {
	    if constexpr (f_dynamic) {
	        if (n == 1) {
	            r = b ;
	        } else if (n == 2) { /* common case */
	            r = b * b ;
	        } else if (n > 2) {
	            cint	t = ipow(b,(n/2)) ;
	            if ((n&1) == 0) {
		        r = (t * t) ;
	            } else {
		        r = b * (t * t) ;
	            }
	        } /* end if */
	    } else {
	        for (int i = 0 ; i < n ; i += 1) {
	            r *= b ;
	        } /* end for */
	    } /* end if-constexpr (f_dynamic) */
	} /* end if (base-specialization) */
	return r ;
}
/* end subroutine (ipow) */


/* local subroutines */

static int ipow10(int n) noex {
	int		r = 0 ;
	if (n < 9) {
	    r = b20tab[n] ;
	} /* end if */
	return r ;
}
/* end subroutine (ipow10) */



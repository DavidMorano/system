/* binexp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* (floating) binary-exponential function */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	binexpf
	binexp
	binexpl

	Description:
	We calculate the binary exponential of a given base (double
	float) and an integral-valued exponent.

	Synopsis:
	float		binexpf(float x,int n) noex
	double		binexp(double x,int n) noex
	longdouble	binexpl(longdouble x,int n) noex

	Arguments:
	x	base
	n	exponent

	Returns:
	-	the result

	This function just calculates:
		x ** n
	or (per preference):
		exp(x,n)
	
*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cmath>			/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"binexp.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename T> static T binexpx(T x,int n) noex {
	T	res = -1 ; /* return-value */
	if (n >= 0) {
	    switch (n) {
	    case 0:
	        res = 1.0 ;
		break ;
	    case 1:
		res = x ;
		break ;
	    case 2:
		res = x*x ;
		break ;
	    case 3:
		res = x*x*x ;
		break ;
	    case 4:
		{
		    const T t = x*x ;
		    res = t*t ;
		}
		break ;
	    default:
		{
		    const T t = binexpx(x,(n/2)) ;
		    res = (n&1) ? x*t*t : t*t ;
		}
		break ;
	    } /* end switch */
	}
	return res ;
} /* end subroutine-template (binexpx) */


/* local variables */


/* exported variables */


/* exported subroutines */

float		binexpf	(float x,int n)		noex {
    	return binexpx(x,n) ;
}
double		binexp	(double x,int n)	noex {
    	return binexpx(x,n) ;
}
longdouble	binexpl	(longdouble x,int n)	noex {
    	return binexpx(x,n) ;
}



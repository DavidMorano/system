/* binexp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* (floating) binary-exponential function */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This was originally written.

*/

/* Copyright Â© 2004 David AÂ­DÂ­ Morano.  All rights reserved. */

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
	long double	binexpl(long double x,int n) noex

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
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cmath>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>

#include	"binexp.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename T> static T binexpx(T x,int n) noex {
	T	v = -1 ;
	if (n >= 0) {
	    switch (n) {
	    case 0:
	        v = 1.0 ;
		break ;
	    case 1:
		v = x ;
		break ;
	    case 2:
		v = x*x ;
		break ;
	    case 3:
		v = x*x*x ;
		break ;
	    case 4:
		{
		    const T t = x*x ;
		    v = t*t ;
		}
		break ;
	    default:
		{
		    const T t = binexpx(x,(n/2)) ;
		    v = (n&1) ? x*t*t : t*t ;
		}
		break ;
	    } /* end switch */
	}
	return v ;
}
/* end subroutine-template (binexpx) */


/* local variables */


/* exported variables */


/* exported subroutines */

float		binexpf(float x,int n) noex {
    	return binexpx(x,n) ;
}
double		binexp(double x,int n) noex {
    	return binexpx(x,n) ;
}
long double	binexpl(long double x,int n) noex {
    	return binexpx(x,n) ;
}



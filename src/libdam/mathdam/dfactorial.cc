/* dfactorial SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* floating-pint factorial function */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-10-09, David A­D­ Morano
	This was adopted from some a previous implementation of
	this subroutine.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	dfactorial
	ldfactorial

	Description:
	We calculate the (floating) factorial of the given (integer)
	number.

	Synopsis:
	double		dfactorial(int n) noex
	long double	ldfactorial(int n) noex

	Arguments:
	n	number to return factorial value for

	Returns:
	-	the factorial result (as a |double|)

	Notes:
	The original Fibonacci function:

	double factorial(int n) noex {
	    return (n * dfactorial(n-1)) ;
	}

	Stirling's approximation:
	f(n) = sqrt(2.0 * pi * n) * pow((n / e),n)

	Gamma function:
	f(n) = gamma(n + 1)

	Note that the C++ function |tgamma(3c++)| needs to be
	imported from the C++ |std| namespace because the C++
	compiler (whatever it may be) is too stupid to deal with
	disambiguating which version of the function (|double| or
	|long double|) to use otherwise (without the namespace
	importation).  This should be corrected with the compiler
	implementation, but who has the time to deal with that?

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cmath>			/* |tgamma(3m++)| */
#include	<numbers>		/* |pi(3c++)| + |e(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<binexp.h>
#include	<localmisc.h>

#include	"dfactorial.h"


/* local defines */


/* impored namespaces */

using std::numbers::pi ;		/* constant (C++20) */
using std::numbers::e ;			/* constant (C++20) */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename T> static T factorialx(int n) noex {
	T	v = -1.0 ;
	if (n >= 0) {
	    switch (n) {
	    case 0:
	    case 1:
	        v = 1.0 ;
		break ;
	    case 2:
		v = 2.0 ;
		break ;
	    case 3:
		v = 6.0 ;
		break ;
	    default:
		{
		    const T x = (T) (n + 1) ;
		    v = tgamma(x) ;
		}
		break ;
	    } /* end switch */
	}
	return v ;
}
/* end subroutine-template (factorialx) */


/* local variables */


/* exported variables */


/* exported subroutines */

double		dfactorial(int n) noex {
    	return factorialx<double>(n) ;
}

long double	ldfactorial(int n) noex {
    	return factorialx<long double>(n) ;
}



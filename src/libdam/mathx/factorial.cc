/* factorial SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* (integer) factorial function */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	factorial

	Descrptor:
	We calculate the factorial of the given number.

	Synopsis:
	long factorial(int n) noex

	Arguments:
	n	number to return factorial value for

	Returns:
	-	the factorial result of the input

	Notes:
	The original Factorial function:

	long factorial(int n) {
	    return (n * factorial(n-1)) ;
	}


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
	The original factorial function:

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

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cmath>			/* |tgamma(3m++)| */
#include	<numbers>		/* |pi(3c++)| + |e(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"factorial.h"


/* local defines */


/* impored namespaces */

using std::tgamma ;			/* subroutine */
using std::numbers::pi ;		/* constant (C++20) */
using std::numbers::e ;			/* constant (C++20) */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

constexpr long		facts[] = {
	0x0000000000000001, 0x0000000000000001, 
	0x0000000000000002, 0x0000000000000006,
	0x0000000000000018, 0x0000000000000078, 
	0x00000000000002d0, 0x00000000000013b0,
	0x0000000000009d80, 0x0000000000058980, 
	0x0000000000375f00, 0x0000000002611500,
	0x000000001c8cfc00, 0x000000017328cc00, 
	0x000000144c3b2800, 0x0000013077775800,
	0x0000130777758000, 0x0001437eeecd8000, 
	0x0016beecca730000, 0x01b02b9306890000,
	0x21c3677c82b40000
} ; /* end array (facts) */

template<typename T> static T factorialx(int n) noex {
	T	v = -1.0 ;
	if (n >= 0) {
	    if (n < nelem(facts)) {
		v = (T) facts[n] ;
	    } else {
		const T x = (T) (n + 1) ;
		v = tgamma(x) ;
	    } /* end if */
	}
	return v ;
}
/* end subroutine-template (factorialx) */


/* exported variables */


/* exported subroutines */

long factorial(int n) noex {
	long		v = -1 ;
	if (n >= 0) {
	    cint	ne = nelem(facts) ;
	    if (n < ne) {
	        v = facts[n] ;
	    }
	}
	return v ;
}
/* end subroutine (factorial) */

double		dfactorial(int n) noex {
    	return factorialx<double>(n) ;
}

long double	ldfactorial(int n) noex {
    	return factorialx<long double>(n) ;
}



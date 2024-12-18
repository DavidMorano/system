/* dfibonacci SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* floating-point (double) Fibonacci function */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This was adopted from some previous subroutine that performed
	the same or similar function.

	= 2025-12-15, David A�D� Morano
	I (finally) revised this little (old) function to use C++20
	math constants (new w/ C++20).  This was not a big deal and
	was likely not very important.  It did save a tiny bit of
	module-load-time static-constant initialization.  Before
	C++20 I computed (at module-load-time) the constants (|double|
	and |long double|) for the Golden-Ratio (phi).

*/

/* Copyright � 1998,2024 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dfifonacci
	ldfifonacci

	Description:
	We calculate the (floating) Fibonacci of the given number.

	Synopsis:
	double		dfibonacci(int n) noex
	long double	ldfibonacci(int n) noex

	Arguments:
	n	number to return Fibonacci value for

	Returns:
	-	the Fibonacci of the input

	Notes:
	The original Fibonacci function:
	double dfibonacci(int n) noex {
	    return dfibonacci(n-1) + dfibonacci(n-2) ;
	}

	Floating-point (double) version:
	constexpr double	phi = ((1.0 + sqrt(5.0)) / 2.0) ;
	constexpr double	num = binexp(phi,n) - binexp((1.0 - phi),n) ;
	constexpr double	den = sqrt(5.0) ;
	dfibonacci(n) = floor(num / den) ;

	Note that the variable 'phi' above is the Golden-Number.

	There is no |float|-type version of this subroutine.

	Note that for C++ the math functions (see my use of
	|floor(3m++)| below) need to be imported from the C++ |std|
	namespace, otherwise the stupid-(ass) compiler will not
	properly disambiguate which version of the function (either
	|double| or |long double|) to call.  We (poor) programmers
	have to deal with these sort of shoddy compiler issues.
	Some will argue that this problem is justified because only
	the C20 language (and not C++20) understands how to
	disambiguate the CMATH functions based on the argument type.
	My response: to you C++ compilers out there -- grow up and
	just deal with it!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cmath>			/* |floor(3m++)| */
#include	<numbers>		/* C++20 math consants */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<binexp.h>
#include	<localmisc.h>

#include	"dfibonacci.h"


/* local defines */


/* imported namespaces */

using std::floor ;			/* subroutine-template (req by C++) */
using std::numbers::phi ;		/* constant (C++20) */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename T> static T fibonaccix(T phi,T den,int n) noex {
	T	v = -1.0 ;
	if (n >= 0) {
	    switch (n) {
	    case 0:
		v = 0.0 ;
		break ;
	    case 1:
	    case 2:
	        v = 1.0 ;
		break ;
	    case 3:
	        v = 2.0 ;
		break ;
	    case 4:
	        v = 3.0 ;
		break ;
	    default:
		{
	            const T num = binexp(phi,n) - binexp((1.0 - phi),n) ;
	            v = floor(num / den) ;
		}
		break ;
	    } /* end switch */
	}
	return v ;
}
/* end subroutine-template (fibonaccix) */


/* local variables */

#ifdef	COMMENT
constexpr double	phi = ((1.0 + sqrt(5.0)) / 2.0) ;   /* prior C++20 */
constexpr long double	phil = ((1.0 + sqrtl(5.0)) / 2.0) ; /* prior C++20 */
#endif

constexpr long double	phil = std::numbers::phi_v<long double> ; /* C++20 */

constexpr double	den = sqrt(5.0) ;
constexpr long double	denl = sqrtl(5.0) ;


/* exported variables */


/* exported subroutines */

double		dfibonacci(int n) noex {
    	return fibonaccix(phi,den,n) ;
}

long double	ldfibonacci(int n) noex { 
    	return fibonaccix(phil,denl,n) ;
}



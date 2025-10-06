/* fibonacci SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Fibonacci function */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This was originally written.

	= 2024-12-15, David A­D­ Morano
	I (finally) revised this little (old) function to use C++20
	math constants (new w/ C++20).  This was not a big deal and
	was likely not very important.  It did save a tiny bit of
	module-load-time static-constant initialization.  Before
	C++20 I computed (at module-load-time) the constants (|double|
	and |long double|) for the Golden-Ratio (phi).

*/

/* Copyright © 1998,2024 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	fibonacci

	Description:
	This subroutine implements the Fibonacci function.  We
	calculate the result using recursion (so watch out for large
	inputs -> stack overflow!).

	Synopsis:
	long fibonacci(int n) noex

	Arguments:
	n	number to return Fibonacci value for

	Returns:
	-	the Fibonacci result of the input

	Notes:
	The original Fibonacci function:

	long fibonacci(int n) noex {
	    long	v = 0 ;
	    if ((n == 1) || (n == 2)) {
	        n = 1 ;
	    } else (n > 2) {
	        v = fibonacci(n - 1) + fibonacci(n - 2) ;
	    }
	    return v ;
	}

	Note that when putting the result into a 32-bit unsigned
	integer (which is what we are doing here) the largest valued
	input (domain) of the Fibonacci function that can be
	represented in the result is 47.  An input value of 48
	overflows the 32-bit unsigned integer result.  For this
	reason, the table of values below is only 48 entries, with
	the last entry (#47) storing the last representable result
	for a 32-bit unsigned integer.


	Name:
	dfifonacci
	lfifonacci

	Description:
	We calculate the (floating) Fibonacci of the given number.

	Synopsis:
	double		dfibonacci(int n) noex
	long double	lfibonacci(int n) noex

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

	Reminder-C++-math:
	Just a reminder that the standard C++ math functions (and
	the standard C-lang functions) are not function-templates
	but are rather function-overloads.  So (what?) ... they have
	to called as functions and not as function templates.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cmath>			/* |floor(3m++)| + |sqrt(3c++)| */
#include	<numbers>		/* C++20 math constants */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"binexp.h"
#include	"fibonacci.h"


/* local defines */


/* imported namespaces */

using std::sqrt ;			/* subroutine-template (req by C++) */
using std::floor ;			/* subroutine-template (req by C++) */
using std::numbers::phi ;		/* constant (C++20) */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

constexpr uint		fibotab[] = {
	0x00000000, 0x00000001, 0x00000001, 0x00000002,
	0x00000003, 0x00000005, 0x00000008, 0x0000000d,
	0x00000015, 0x00000022, 0x00000037, 0x00000059,
	0x00000090, 0x000000e9, 0x00000179, 0x00000262,
	0x000003db, 0x0000063d, 0x00000a18, 0x00001055,
	0x00001a6d, 0x00002ac2, 0x0000452f, 0x00006ff1,
	0x0000b520, 0x00012511, 0x0001da31, 0x0002ff42,
	0x0004d973, 0x0007d8b5, 0x000cb228, 0x00148add,
	0x00213d05, 0x0035c7e2, 0x005704e7, 0x008cccc9,
	0x00e3d1b0, 0x01709e79, 0x02547029, 0x03c50ea2,
	0x06197ecb, 0x09de8d6d, 0x0ff80c38, 0x19d699a5,
	0x29cea5dd, 0x43a53f82, 0x6d73e55f, 0xb11924e1
} ; /* end array (fibotab) */

template<typename T> constexpr T five = T{5.0} ;

template<typename T> static T fibonaccix(int n) noex {
    	static constexpr T	den = sqrt(five<T>) ;
	static constexpr T	aphi = std::numbers::phi_v<T> ;
	T		v = -1.0 ;
	if (n >= 0) {
	    if (n < nelem(fibotab)) {
	        v = (T) fibotab[n] ;
	    } else {
	        const T num = binexp(aphi,n) - binexp((1.0 - aphi),n) ;
	        v = floor(num / den) ;
	    } /* end if */
	}
	return v ;
}
/* end subroutine-template (fibonaccix) */


/* local variables */


/* exported variables */


/* exported subroutines */

long fibonacci(int n) noex {
	cint		ntab = nelem(fibotab) ;
	long		v = -1 ;
	if (n < ntab) {
	    v = fibotab[n] ;
	} else {
	    v = fibonacci(n - 1) * fibonacci(n - 2) ;
	}
	return v ;
}
/* end subroutine (fibonacci) */

double dfibonacci(int n) noex {
    	return fibonaccix<double>(n) ;
}

longdouble lfibonacci(int n) noex { 
    	return fibonaccix<longdouble>(n) ;
}



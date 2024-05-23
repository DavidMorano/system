/* dfibonacci SUPPORT */
/* lang=C++98 */

/* floating-point Fibonacci function */
/* version %I% last-modified %G% */


/* revision history:

	= 2018-10-08, David A.D. Morano
	This was rewritten (and in C++11).

*/

/* Copyright � 2018 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dfifonacci

	Description:
	We calculate the (floating) Fibonacci of the given number.

	Synopsis:
	double dfibonacci(int n)

	Arguments:
	n	number to return Fibonacci value for

	Returns:
	-	the Fibonacci of the input

	Notes:
	The original Fibonacci function:
	double dfibonacci(int n)
	{
	    return dfibonacci(n-1) + dfibonacci(n-2) ;
	}

	Floating-point version:
	constexpr double	phi = ((1.0 + sqrt(5.0)) / 2.0) ;
	constexpr double	num = binexp(phi,n) - binexp((1.0 - phi),n) ;
	constexpr double	den = sqrt(5.0) ;
	dfibonacci(n) = floor(num/den) ;

	Note that the variable 'phi' above is the Golden-Number.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>
#include	<cmath>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>

#include	"dfibonacci.h"


/* local defines */


/* external subroutines */

extern "C" double	dfibonacci(int) noex ;

extern "C" double	binexp(double,int) noex ;


/* external variables */


/* local structures */


/* foward references */


/* local variables */

constexpr double	phi = ((1.0 + sqrt(5.0)) / 2.0) ;
constexpr double	den = sqrt(5.0) ;


/* exported variables */


/* exported subroutines */

double dfibonacci(int n) noex {
	double		v = -1.0 ;
	if (n >= 0) {
	    v = 0.0 ;
	    if ((n >= 1) && (n <= 2)) {
	        v = 1.0 ;
	    } else if (n == 3) {
	        v = 2.0 ;
	    } else if (n > 3) {
	        double	num = binexp(phi,n) - binexp((1.0 - phi),n) ;
	        v = floor(num / den) ;
	    }
	}
	return v ;
}
/* end subroutine (dfibonacci) */



/* dfactorial SUPPORT */
/* lang=C++20 */

/* floating-pint factorial function */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-10-09, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We calculate the (floating) factorial of the given number.

	Synopsis:
	double dfactorial(int n) noex

	Arguments:
	n	number to return factorial value for

	Returns:
	-	the Fibonacci number of the input


	Notes:
	The original Fibonacci function:

	double factorial(int n) noex {
	    return (n * dfactorial(n-1)) ;
	}

	Note that when putting the result into a 32-bit unsigned
	integer (which is what we are doing here) the largest valued
	input (domain) of the Factorial function that can be
	represented in the result is 12. An input value of 13
	overflows the 32-bit unsigned integer result.

	Floating-point:
	f(n) = floor(1.0 / sqrt(5.0)) * pow(((1.0 + sqrt(5.0)) / 2.0),(n+1))

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cmath>
#include	<climits>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */

extern "C" {
    extern double	fbinexp(double,int) noex ;
}


/* external variables */


/* local structures */


/* foward references */

extern "C" double	dfactorial(double,int) noex ;


/* local variables */

constexpr double	phi = ((1.0 + sqrt(5.0))/2.0) ;


/* exported variables */


/* exported subroutines */

double dfactorial(int n) noex {
	double			v = -1.0 ;
	if (n >= 0) {
	    v = 1.0 ;
	    if (n > 1) {
		v = fbinexp(phi,(n+1)) + 0.5 ;
	    }
	}
	return v ;
}
/* end subroutine (dfactorial) */



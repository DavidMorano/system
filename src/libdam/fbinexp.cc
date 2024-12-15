/* dbinexp SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C99 */

/* (floating) binary-exponential function */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-10-09, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dbinexp

	Description:
	We calculate the binary exponential of a given base (double
	float) and an integral-valued exponent.

	Synopsis:
	double binexp(double x,int n) noex

	Arguments:
	x	base
	n	exponent

	Returns:
	-	the result

	This function just calculates:
		x ** n
	
*******************************************************************************/

#include	<envstandards.h>
#include	<climits>
#include	<cmath>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

double dbinexp(double x,int n) noex {
	double		v = -1 ;
	if (n >= 0) {
	    v = 1.0 ;
	    if (n == 1) {
		v = x ;
	    } else if (n == 2) {
		v = x*x ;
	    } else if (n > 2) {
		double	t = dbinexp(x,(n/2)) ;
		v = (n&1) ? x*t*t : t*t ;
	    }
	}
	return v ;
}
/* end subroutine (dbinexp) */



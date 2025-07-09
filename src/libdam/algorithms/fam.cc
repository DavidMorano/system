/* fam SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Floating-Point Arithmetic Mean */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-03-01, David A­D­ Morano
	I first created this for calculating stastics for Levo stuff.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	fam (Floating-Point Arithmetic Mean)

	Description:
	This subroutine computes the arithmetic mean on a set of
	(floating-point) numbers.

	Synopsis:
	double fam(duoble *a,int n) noex

	Arguments:
	a	array of double floats of the numbers to find the HM of
	n	number of elements in the array

	Returns:
	-	the arithmetic mean

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
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

double fam(double *a,int n) noex {
	double		ans = 0.0 ;
	if (n > 0) {
	    double	d = double(n) ;
	    double 	sum = 0.0 ;
	    for (int i = 0 ; i < n ; i += 1) {
	        sum += a[i] ;
	    }
	    ans = (sum/d) ;
	}
	return ans ;
}
/* end subroutine (fam) */



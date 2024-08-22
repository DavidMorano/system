/* fhm SUPPORT */
/* lang=C++20 */

/* floating point harmonic mean */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-03-01, David A­D­ Morano
	I first created this for calculating HMs for Levo stuff.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	fhm

	Description:
	This subroutine computes the harmonic mean on a set of
	numbers.

	Synopsis:
	double fhm(double a,int n) noex

	Arguments:
	a	array of double floats of the numbers to find the HM of
	n	number of elements in the array

	Returns:
		the harmonic mean

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>

#include	"fhm.h"


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

double fhm(double *a,int n) noex {
	double		ans = 0.0 ;
	if (n > 0) {
	    double	fn = (double) n ;
	    double	e = 0.00001 ;
	    double	d = 0.0 ;
	    for (int i = 0 ; i < n ; i += 1) {
	        if (a[i] < e) break ;
	        d += (1.0 / a[i]) ;
	    } /* end for */
	    if (d >= e) {
	        ans = (fn / d) ;
	    }
	}
	return ans ;
}
/* end subroutine (fhm) */



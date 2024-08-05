/* ndig SUPPORT */
/* lang=C++20 */

/* digit determination and handling */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	ndig
	ndigmax

	Description:
	These subroutines provide some special digit (decimal)
	processing for special floating point uses (without having
	to link in the math library).

	Synopsis:
	int ndig(double *la,int n) noex

	Arguments:
	la		array of double floating-point numbers
	n		number of elements in array (to test)

	Returns:
	-		maximum number of digits across all of the 
			floating-point numbers in the array

	Synopsis:
	int ndigmax(double *la,int n,int m) noex

	Arguments:
	la		array of double floating-point numbers
	n		number of elements in array (to test)
	m		maxium integer power of elemen to consider

	Returns:
	-		maximum number of digits across all of the 
			floating-point numbers in the array

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>

#include	"ndig.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */

static int	digs(double) noex ;


/* local variables */


/* external variables */


/* external subroutines */

int ndig(double *la,int n) noex {
	int		m = 0 ;
	for (int i = 0 ; i < n ; i += 1) {
	    cint	t = digs(la[i]) ;
	    if (t > m) m = t ;
	} /* end for */
	return m ;
}
/* end subroutine (ndig) */

int ndigmax(double *la,int n,int m) noex {
	double		c = 1.0 ;
	for (int i = 0 ; i < m ; i += 1) {
	    c = c * 10.0 ;
	}
	c = (c - 0.1) ;
	for (int i = 0 ; i < n ; i += 1) {
	    if (la[i] > c) la[i] = c ;
	} /* end for */
	return m ;
}
/* end subroutine (ndigmax) */


/* local subroutines */

static int digs(double f) noex {
	int		i{} ;
	for (i = 0 ; (f > 1.0) ; i += 1) {
	    f = f / 10.0 ;
	}
	return i ;
}
/* end subroutine (digs) */



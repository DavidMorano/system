/* logbase SUPPORT */
/* lang=C20 */

/* calculate the log of a number given the base also */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-03-04, David A.D. Morano
	This was written from scratch.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	logbase

	Description:
	Find the log-base-(b) of a number (v).

	Synopsis:
	double logbase(double b,double v) noex

	Arguments:
	b		base
	v		number to take the log-base-(b) of

	Returns:
	-		result

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<math.h>
#include	<clanguage.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

double logbase(double b,double v) noex {
	const double	n = log(v) ;
	const double	d = log(b) ;
	return (n/d) ;
}
/* end subroutine (logbase) */



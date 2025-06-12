/* logbase SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* calculate the log of a number given the base also */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-03-04, David A-D- Morano
	This was written from scratch.

*/

/* Copyright © 2003 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	logbasef
	logbase
	logbasel

	Description:
	Find the log-base-(b) of a number (v).

	Synopsis:
	float		logbase(float b,float v) noex
	double		logbase(double b,double v) noex
	long double	logbase(long double b,long double v) noex

	Arguments:
	b		base
	v		number to take the log-base-(b) of

	Returns:
	-		result

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cmath>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>

#include	"logbase.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename T>
T logbasex(T b,T v) noex {
	const T		n = log(v) ;
	const T		d = log(b) ;
	return (n / d) ;
}
/* end subroutine-template (logbasef) */


/* local variables */


/* exported variables */


/* exported subroutines */

float logbasef(float b,float v) noex {
	return logbasex(b,v) ;
}
/* end subroutine (logbasef) */

double logbase(double b,double v) noex {
	return logbasex(b,v) ;
}
/* end subroutine (logbase) */

long double logbasel(long double b,long double v) noex {
	return logbasex(b,v) ;
}
/* end subroutine (logbasel) */



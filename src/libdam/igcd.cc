/* igcd SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* integer Greatest Common Divisor (GCD) */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */

/* revision history:

	= 2004-03-01, David A­D­ Morano
	This code was originally written.  

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	igcd

	Description:
        This subroutine calculates the Greastest Common Divisor (GCD) of
        two integer numbers.

	Synopsis:
	int igcd(int a,int b) noex

	Arguments:
	a	number
	b	number

	Returns:
	-	GCD(a,b)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<localmisc.h>		/* more types */


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int igcd(int a,int b) noex {
	for (int r ; (r = (a % b)) > 0 ; ) {
	    a = b ;
	    b = r ;
	} /* end while */
	return b ;
}
/* end subroutine (igcd) */



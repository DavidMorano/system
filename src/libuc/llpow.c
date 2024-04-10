/* llpow SUPPORT */
/* lang=C99 */


/* revision history:

	= 2013-07-11, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2013 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Find an integral power of a given number.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<localmisc.h>


/* name-spaces */


/* external subroutines */


/* global variables */


/* local structures (and methods) */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

longlong_t llpow(longlong_t b,int n) noex {
	longlong_t	v = 1 ;
	for (int i = 1 ; i < n ; i += 1) {
 	    v += v ;
	}
	return v ;
}
/* end subroutine (llpow) */



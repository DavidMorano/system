/* isprime SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* determine if a number is prime or not */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	isprime

	Description:
	This little subroutine detemines if the given number is
	prime or not.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<math.h>		/* math functions */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<localmisc.h>

#include	"ismisc.h"


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

bool isprime(uint n) noex {
	bool		f = true ;
	if (n != 2) {
	    f = false ;
	    if ((n != 1) && ((n & 1) == 1)) {
	        double	fn = n ;
	        double	fr ;
	        uint	root ;
	        fr = sqrt(fn) ;
	        root = (uint) ceil(fr) ; /* safety due to floating errors */
	        for (uint i = 3 ; i <= root ; i += 1) {
	            f = ((n % i) != 0) ;
	            if (!f) break ;
	        } /* end for */
	    } /* end if (number not '1' and not odd) */
	} /* end if (number not '2') */
	return f ;
}
/* end subroutine (isprime) */



/* ipow SUPPORT */
/* lang=C++20 */

/* return integer-power */
/* version %I% last-modified %G% */

#define	CF_DYNAMIC	1		/* dynamic programming */

/* revision history:

	= 2004-03-01, David A­D­ Morano
	This subroutine was originally written.  

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

        This subroutine calculates and returns an integer power for a given
        base.

	Synopsis:
	int ipow(int b,int p) noex

	Arguments:
	b	base
	p	power

	Returns:
	-	result (b**p)

	Notes: 
	Only defined for positive exponents.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>		/* 'LONG' type */

#include	"ipow.h"


/* local defines */

#ifndef	CF_DYNAMIC
#define	CF_DYNAMIC	1		/* dynamic programming */
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr bool		f_dynamic = CF_DYNAMIC ;


/* exported variables */


/* exported subroutines */

int ipow(int b,int n) noex {
	int		r = 1 ;
	if constexpr (f_dynamic) {
	    if (n == 1) {
	        r = b ;
	    } else if (n == 2) { /* common case */
	        r = b*b ;
	    } else if (n > 2) {
	        int	t = ipow(b,(n/2)) ;
	        if ((n&1) == 0) {
		    r = (t*t) ;
	        } else {
		    r = b*(t*t) ;
	        }
	    }
	} else {
	    for (int i = 0 ; i < n ; i += 1) {
	        r *= b ;
	    }
	} /* end if-constexpr (f_dynamic) */
	return r ;
}
/* end subroutine (ipow) */



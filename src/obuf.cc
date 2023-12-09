/* obuf */
/* lang=C++98 */

/* Output Buffer (object) */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This was carved out of an existing subroutine.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We facilitate output buffering.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<new>
#include	<localmisc.h>

#include	"obuf.hh"


/* local defines */


/* default name spaces */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int obuf::adv(int al) noex {
	cint		sl = b.size() ;
	int		rl = 0 ;
	if (al > 0) {
	    if (sl > (oi+al)) {
	        rl = (sl - oi) ;
	        oi += rl ;
	    } else {
	        rl = (sl - oi) ;
	        oi += rl ;
	        if (rl == 0) {
	            b.clear() ;
	            oi = 0 ;
	        }
	    } /* end if */
	} else if (al < 0) {
	    if (sl > oi) {
	        rl = (sl - oi) ;
	        oi += rl ;
	    } else {
	        b.clear() ;
	        oi = 0 ;
	    }
	} /* end if */
	return rl ;
}
/* end subroutine (obuf::adv) */



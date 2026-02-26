/* satcount SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* saturating counter */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-03-15, David A­D­ Morano
	This object module was created for Levo research.  It is a
	value predictor.  This is not coded as hardware.  It is
	like Atom analysis subroutines!

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	satcount

	Description:
	This subroutine performs a saturdated counter operation.
	This subroutine is special to the Levo branch-predictor
	siumlators.  If you find other uses for this, more power
	to you.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<localmisc.h>

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

uint satcount(uint v,uint n,int f_up) noex {
	uint	r ;
	if (f_up) {
	    r = (v == (n - 1)) ? v : (v + 1) ;
	} else {
	    r = (v == 0) ? 0 : (v - 1) ;
	}
	return r ;
}
/* end subroutine (satcount) */



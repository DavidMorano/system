/* uerrtimer SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* error-timer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	errtimer

	Description:
	This object actually provides a counter for counting how
	many times it is called (after sleeping on each call).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>

#include	"errtimer.hh"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

#ifndef	SUBROUTINE_MSLEEP
#define	SUBROUTINE_MSLEEP
EXTERNC_begin
extern int	msleep(int) noex ;
EXTERNC_end
#endif


/* external variables */


/* local variables */


/* exported variables */


/* exported subroutines */

const reterr errtimer::operator () (int ars) noex {
	reterr		r(ars,(mto == 0)) ;
	rs = ars ;
	if (mto-- > 0) {
	    if ((rs = msleep(1)) < 0) {
		r(true) ;
	    }
	}
	return r ;
}
/* end method (errtimer::operator) */

errtimer::operator reterr () noex {
	reterr		r(rs,(mto == 0)) ;
	return r ;
}
/* end method (errtimer::operator) */



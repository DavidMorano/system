/* timecount SUPPORT */
/* lang=C++20 */

/* count-down timer */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

	This object provides a florified count-down timer.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<usystem.h>

#include	"timecount.hh"


timecount::operator int () noex {
	    int		rs = SR_TIMEDOUT ;
	    int		f = true ;
	    if (to-- > 0) {
		f = false ;
		if ((rs = msleep(1)) > 0) {
		    rs = SR_OK ;
		}
	    } /* end if (counting down) */
	    return (rs >= 0) ? f : rs ;
} 
/* end method (timecount::operator) */



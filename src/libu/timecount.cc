/* timecount SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* count-down timer */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	timecount

	Description:
	This object provides a glorified count-down timer.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */

#include	"timecount.hh"


/* local defines */


/* external subroutines */


/* external variables */


/* local variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

timecount::operator int () noex {
        int         rs = SR_TIMEDOUT ;
        int         f = true ;
        if (to-- > 0) {
            f = false ;
            if ((rs = msleep(1)) > 0) {
                rs = SR_OK ;
            }
        } /* end if (counting down) */
        return (rs >= 0) ? f : rs ;
} /* end method (timecount::operator) */



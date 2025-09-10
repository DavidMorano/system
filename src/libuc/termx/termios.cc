/* termios SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<termios.h>
#include	<unistd.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>

#include	"termios_cf.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int termios_cfsetispeed(TERMIOS *tp,speed_t speed) noex {
	int		rs = SR_FAULT ;
	if (tp) {
	    repeat {
	        if ((rs = cfsetispeed(tp,speed)) < 0) {
		    rs = (- errno) ;
	        }
	    } until ((rs >= 0) || ((rs != SR_INTR) && (rs != SR_NOMEM))) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (termios_cfsetispeed) */

int termios_cfsetospeed(TERMIOS *tp,speed_t speed) noex {
	int		rs = SR_FAULT ;
	if (tp) {
	    repeat {
	        if ((rs = cfsetospeed(tp,speed)) < 0) {
	            rs = (- errno) ;
	        }
	    } until ((rs >= 0) || ((rs != SR_INTR) && (rs != SR_NOMEM))) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (termios_cfsetospeed) */



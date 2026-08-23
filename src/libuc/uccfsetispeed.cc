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
#include	<sys/types.h>		/* POSIX® */
#include	<termios.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"termios_cf.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int termios_cfsetispeed(TERMIOS *tp,speed_t speed) noex {
	int		rs = SR_FAULT ;
	if (tp) {
	    repeat {
	        if ((rs = cfsetispeed(tp,speed)) < 0) {
		    rs = (neg errno) ;
	        }
	    } until ((rs >= 0) || ((rs != SR_INTR) && (rs != SR_NOMEM))) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (termios_cfsetispeed) */

int termios_cfsetospeed(TERMIOS *tp,speed_t speed) noex {
	int		rs = SR_FAULT ;
	if (tp) {
	    repeat {
	        if ((rs = cfsetospeed(tp,speed)) < 0) {
	            rs = (neg errno) ;
	        }
	    } until ((rs >= 0) || ((rs != SR_INTR) && (rs != SR_NOMEM))) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (termios_cfsetospeed) */



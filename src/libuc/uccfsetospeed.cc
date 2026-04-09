/* uccfsetospeed */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<termios.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<localmisc.h>


/* exported variables */


/* exported subroutines */

int uc_cfsetospeed(TERMIOS *tp,speed_t speed) noex {
	int		rs = SR_FAULT ;
	if (tp) {
	    if ((rs = cfsetospeed(tp,speed)) < 0) {
	        rs = (- errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_cfsetospeed) */



/* uccfsetospeed */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debugging */

/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<termios.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<poll.h>		/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */


/* exported variables */


/* exported subroutines */

int uc_cfsetospeed(TERMIOS *tp,speed_t speed) noex {
	int		rs = SR_FAULT ;
	if (tp) ylikely {
	    if ((rs = cfsetospeed(tp,speed)) < 0) {
	        rs = (neg errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_cfsetospeed) */



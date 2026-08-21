/* unanosleep SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<ctime>			/* CSTD |nanosleep(2)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */


/* local defines */


/* external subroutines */


/* external variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int u_nanosleep(CTIMESPEC *tsp,TIMESPEC *rtsp) noex {
	int		rs = SR_FAULT ;
	if (tsp) {
	    if ((rs = nanosleep(tsp,rtsp)) < 0) {
	        rs = (neg errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (u_nanosleep) */



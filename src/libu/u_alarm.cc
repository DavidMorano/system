/* u_alarm */

/* send a signal to a process */
/* translation layer interface for UNIX® equivalents */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<limits.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>		/* for 'uint' type */


/* local defines */


/* exported variables */


/* exported subroutines */

int u_alarm(const uint secs) noex {
	uint		rem ;
	int		rs ;
	int		sec = (secs & INT_MAX) ;

	if ((rem = alarm(sec)) < 0) {
	    rs = (- errno) ;
	}

	if (rs >= 0) rs = (rem & INT_MAX) ;
	return rs ;
}
/* end subroutine (u_alarm) */



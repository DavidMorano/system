/* u_stime */

/* translation layer interface for UNIX® equivalents */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<unistd.h>		/* <- the money shot */
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* exported subroutines */

int u_stime(time_t *tp) noex {
	int		rs = SR_FAULT ;
	if (rp) {
	    if ((rs = stime(rp)) < 0) {
		rs = (- errno) ;
	    }
	}
	return rs ;
}
/* end subroutine (u_stime) */



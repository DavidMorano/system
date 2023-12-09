/* u_getgroups */
/* lang=C20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* exported subroutines */

int u_getgroups(int n,gid_t *a) noex {
	int		rs = SR_FAULT ;
	if (a) {
	    if ((rs = getgroups(n,a)) < 0) {
	        rs = (- errno) ;
	    }
	}
	return rs ;
}
/* end subroutine (u_getgroups) */



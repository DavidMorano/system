/* u_wait */

/* translation layer interface for UNIX� equivalents */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */


#include	<envstandards.h>

#include	<sys/types.h>
#include	<sys/uio.h>
#include	<sys/wait.h>
#include	<unistd.h>
#include	<poll.h>
#include	<errno.h>

#include	<usystem.h>
#include	<localmisc.h>


/* exported subroutines */


int u_wait(int *sp)
{
	pid_t		rpid ;
	int		rs ;

	repeat {
	    rs = SR_OK ;
	    if ((rpid = wait(sp)) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;

	if (rs >= 0) rs = (int) rpid ;

	return rs ;
}
/* end subroutine (u_wait) */



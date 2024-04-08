/* u_utimes */

/* translation layer interface for UNIX� equivalents */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/wait.h>
#include	<sys/time.h>
#include	<utime.h>
#include	<unistd.h>
#include	<poll.h>
#include	<errno.h>
#include	<time.h>

#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* exported subroutines */


int u_utimes(cchar *fname,const struct timeval *tvp)
{
	int		rs ;

	repeat {
	    rs = SR_OK ;
	    if (utimes(fname,tvp) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;

	return rs ;
}
/* end subroutine (u_utimes) */



/* u_msgrcv */

/* translation layer interface for UNIX� equivalents */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */


#include	<envstandards.h>

#include	<sys/types.h>
#include	<sys/wait.h>
#include	<sys/msg.h>
#include	<unistd.h>
#include	<poll.h>
#include	<errno.h>

#include	<vsystem.h>
#include	<localmisc.h>


/* local defines */

#define	TO_NOSPC	5


/* exported subroutines */


int u_msgrcv(msqid,msgp,msgsz,msgtype,msgflag)
int	msqid ;
void	*msgp ;
int	msgsz ;
long	msgtype ;
int	msgflag ;
{
	int		rs ;

	repeat {
	    if ((rs = msgrcv(msqid,msgp,msgsz,msgtype,msgflag)) < 0) {
	        rs = (- errno) ;
	    }
	} until (rs != SR_INTR) ;

	return rs ;
}
/* end subroutine (u_msgrcv) */



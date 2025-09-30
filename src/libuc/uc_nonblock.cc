/* uc_nonblock SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX� library-3c */
/* set the NONBLOCK file descriptor open-flag */


/* revision history:

	= 1998-04-13, David A�D� Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_nonblock(int fd,int f) noex {
	int		rs ;
	int		f_previous = false ;
	if ((rs = u_fcntl(fd,F_GETFL,0)) >= 0) {
	    int		flflags = rs ;
	    f_previous = (flflags & O_NONBLOCK) ? 1 : 0 ;
	    if (! LEQUIV(f_previous,f)) {
	        if (f) {
	            flflags |= O_NONBLOCK ;
	        } else {
	            flflags &= (~ O_NONBLOCK) ;
		}
	        rs = u_fcntl(fd,F_SETFL,flflags) ;
	    } /* end if (needed a change) */
	} /* end if (u_fcntl) */
	return (rs >= 0) ? f_previous : rs ;
}
/* end subroutine (uc_nonblock) */



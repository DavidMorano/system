/* u_lockf */
/* lang=C20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	u_lockf

	Description:
	This subroutine is the friendly version of the library
	subroutine (system call in some older OSes) that handles
	UNIX® System V style locking (not the stupid |flock()|
	stuff).

	Synopsis:
	int u_lockf(int fd,int cmd,off_t sz) noex

	Arguments:
	fd		file-descriptor
	cmd		command:
				F_LOCK
				F_UNLOCK
				F_TLOCK
				F_TEST
	sz		the size of the area to lock (can be zero)

	Returns:
	>=0		OK
	<0		error

	NOTE:
	Note that the stupid developers of the File Locking mechanism
	un UNIX® System V did not distinguish a real deadlock from
	a temporary lack of system resources. We attempt to make
	up for this screw ball bug in UNIX® with our retries on
	DEADLOCK.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* exported subroutines */

int u_lockf(int fd,int cmd,off_t sz) noex {
	int		rs = SR_INVALID ;
	if (fd >= 0) {
	    int		to_deadlock = utimeout[uto_deadlock] ;
	    int		to_nolck = utimeout[uto_nolck] ;
	    int		to_again = utimeout[uto_again] ;
	    bool	f_exit = false ;
	    repeat {
	        if ((rs = lockf(fd,cmd,sz)) < 0) rs = (- errno) ;
	        if (rs < 0) {
	            switch (rs) {
	            case SR_DEADLK:
	                if (to_deadlock-- > 0) {
	                    msleep(1000) ;
		        } else {
			    f_exit = TRUE ;
		        }
	                break ;
	            case SR_NOLCK:
	                if (to_nolck-- > 0) {
	                    msleep(1000) ;
		        } else {
			    f_exit = TRUE ;
		        }
	                break ;
	            case SR_AGAIN:
	                if (to_again-- > 0) {
	                    msleep(1000) ;
		        } else {
			    f_exit = TRUE ;
		        }
	                break ;
	            case SR_INTR:
	                break ;
    		    default:
		        f_exit = TRUE ;
		        break ;
	            } /* end switch */
	        } /* end if (error) */
	    } until ((rs >= 0) || f_exit) ;
	    if (rs == SR_ACCES) rs = SR_AGAIN ;
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (u_lockf) */



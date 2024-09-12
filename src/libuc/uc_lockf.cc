/* uc_lockf */
/* lang=C20 */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revisions history:

	= 1998-01-15, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Synopsis:
	int uc_lockf(int fd,int cmd,off_t size) noex

	Arguments:
	fd	file descriptor of file to lock
	cmd	command:
			F_ULOCK
			F_WLOCK (F_LOCK)
			F_TWLOCK (F_TLOCK)
			F_TWTEST (F_TEST)
			F_RLOCK
			F_TRLOCK
			F_RTEST

	size		size of region to lock in the file (0="whole file")

	Returns:
	>=0		OK
	<0		error

	NOTE:
	Note that the stupid developers of the File Locking mechanism
	un UNIX® System V did not distinguish a real deadlock from
	a temporary lack of system resources. We attempt to make
	up for this screw ball bug in UNIX® with our retries on
	DEADLOCK.

	Also, watch out for stupid Solaris bugs. They do not want
	a remote file to be memory mapped and locked at the same
	time. They think that they are protecting us against ourselves
	but in reality they are just inept developers who are making
	our lives more miserable with Solaris than it alreay is!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<errno.h>
#include	<usystem.h>
#include	<ucgetpid.h>
#include	<upt.h>
#include	<localmisc.h>


/* local defines */

#define	TO_DEADLOCK	50		/* time-out in seconds */


/* external subroutines */


/* exported subroutines */

int uc_lockf(int fd,int cmd,off_t size) noex {
	int		rs = SR_NOTOPEN ;
	if (fd >= 0) {
	    rs = SR_INVALID ;
	    if ((cmd >= 0) && (cmd <= F_TEST)) {
	        int	to = (TO_DEADLOCK*1000) ;
	        bool	f_exit = false ;
	        repeat {
	            errno = 0 ;
	            if ((rs = lockf(fd,cmd,size)) < 0) rs = (- errno) ;
	            if (rs < 0) {
	                switch (rs) {
	                case SR_DEADLK:
	                    if (to-- > 0) {
	                        msleep(1) ;
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
	} /* end if (open) */
	return rs ;
}
/* end subroutine (uc_lockf) */



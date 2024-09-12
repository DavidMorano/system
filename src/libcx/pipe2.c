/* pipe2 SUPPORT */
/* lang=C20 */

/* check file access for the current process by its effective UID */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pipe2

	Description:
	Enhanced |pipe(2)|.

	Synopsis:
	int pipe2(int pipes[2],int of) noex

	Arguments:
	pipes		array of two integers to receive created pipe FDs
	om		open flags

	Returns:
	>=0		OK
	<0		error in dialing (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<stdlib.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int pipe2(int *pipes,int of) noex {
	int		rs ;
	if ((rs = u_pipe(pipes)) >= 0) {
		const int	f = TRUE ;
		if ((rs >= 0) && (of & O_NONBLOCK)) {
		    if ((rs = uc_nonblock(pipes[0],f)) >= 0) {
		        rs = uc_nonblock(pipes[1],f) ;
		    }
		}
		if ((rs >= 0) && (of & O_CLOEXEC)) {
		    if ((rs = uc_closeonexec(pipes[0],f)) >= 0) {
		        rs = uc_closeonexec(pipes[1],f) ;
		    }
		}
	    if (rs < 0) {
		u_close(pippe[0]) ;
		u_close(pippe[1]) ;
	    }
	} /* end if (u_pipe) */
	if (rs < 0) errno = (-rs) ;
	return rs ;
}
/* end subroutine (pipe2) */



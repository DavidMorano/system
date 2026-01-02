/* uc_piper SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX®Â® library-3c */
/* create pipes but moved up to a minimum FD number */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 1998-03-11, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_piper

	Description:
	This subroutine creates pipes, like |u_pipe(3u)| does, but with 
	resulting FDs that are at or above a minimum FD value.

	Synopsis:
	int uc_piper(int *pipes,int minfd) noex

	Arguments:
	pipes		resulting array of two pipes
	minfd		minimum number where new FD will move to

	Returns:
	>=0		the new FD
	<0		error (system-return)

	Notes:
	In the old days (or on many BSD type UNIX®Â® systems) the
	two resulting pipe ends were unidirectional as follows:

	end	access
	---------------
	0	open for reading
	1	open for writing

	With the advent of SYSV UNIX®Â®, pipes are completely
	bidirectional with both ends suitable for reading and writing.
	Data written to either end is available for reading from
	the opposite end.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_piper(int *pipes,int minfd) noex {
	int		rs = SR_FAULT ;
	if (pipes) {
	    if (((rs = u_pipe(pipes)) >= 0) && (minfd >= 1)) {
	        for (int i = 0 ; (rs >= 0) && (i < 2) ; i += 1) {
	            if (pipes[i] < minfd) {
	                if ((rs = uc_moveup(pipes[i],minfd)) >= 0) {
	                    pipes[i] = rs ;
		        }
	            }
	        } /* end for */
	        if (rs < 0) {
	            u_close(pipes[0]) ;
	            u_close(pipes[1]) ;
	        }
	    } /* end if (created successfully) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_piper) */



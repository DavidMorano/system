/* ucpipeover SUPPORT */
/* charset=ISO8859-1 */
/* lang=C20 */

/* open pipes */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_pipe2

	Description:
	Enhanced |pipe(2)|.

	Synopsis:
	int uc_pipe2(int pipes[2],int of) noex
	int uc_piper(int pipes[2],int of,min) noex

	Arguments:
	pipes		array of two integers to receive created pipe FDs
	om		open flags

	Returns:
	>=0		OK
	<0		error (system-return)

	Name:
	uc_piper

	Description:
	This subroutine creates pipes, like |u_pipe(3u)| does, but with 
	resulting FDs that are at or above a minimum FD value.

	Synopsis:
	int uc_piper(int *pipes,int of,ine minfd) noex

	Arguments:
	pipes		resulting array of two pipes
	of		open-flags
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
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cerrno>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#if	defined(SYSHAS_PIPES) && (SYSHAS_PIPES > 0)
#define	F_PIPES		1
#else
#define	F_PIPES		0
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr bool		f_pipes = F_PIPES ;


/* exported variables */


/* exported subroutines */

int uc_pipe2(int *pipes,int of) noex {
	int		rs ;
	if_constexpr (f_pipes) {
	    rs = u_pipe2(pipes,of) ;
	} else {
	    if ((rs = u_pipe(pipes)) >= 0) {
	        cint	f = true ;
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
		    u_close(pipes[0]) ;
		    u_close(pipes[1]) ;
	        }
	    } /* end if (u_pipe) */
	} /* end if_constexpr (f_pipes) */
	return rs ;
}
/* end subroutine (uc_pipe2) */

int uc_pipes(int *pipes,int of) noex {
	return uc_pipe2(pipes,of) ;
}

int uc_piper(int *pipes,int of,int minfd) noex {
	int		rs = SR_FAULT ;
	if (pipes) {
	    if (((rs = uc_pipe2(pipes,of)) >= 0) && (minfd >= 1)) {
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



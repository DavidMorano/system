/* uc_forkdetached SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-15, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	This subroutine forks the current process but it does so
	with the CHILD signal ignored first.


	Synopsis:
	int uc_forkdetached()

	Arguments:

	Returns:
	>=0		UID of process (as an integer)
	<0		error code (system-return)

        Note: 
	This really accomplishes pretty much nothing, and therefore
	is not even compiled into anything (libraries included).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<signal.h>
#include	<stdlib.h>
#include	<string.h>

#include	<usystem.h>
#include	<storebuf.h>


/* local defines */


/* external subroutines */


/* local variables */


/* forward references */


/* exported variables */


/* exported subroutines */

int uc_forkdetached() noex {
	SIGACTION	osh ;
	SIGACTION	nsh{} ;
	sigset_t	sm ;
	int		rs ;
	int		rs1 ;
	int		pid = 0 ;
	uc_sigemptyset(&sm) ;
	nsh.sa_handler = SIG_IGN ;
	nsh.sa_mask = sm ;
	nsh.sa_flags = (SA_NOCLDWAIT | SA_NOCLDSTOP) ;
	if ((rs = u_sigaction(SIGCLD,&nsh,&osh)) >= 0) {
	    {
	        rs = uc_fork() ;
	        pid = rs ;
	    }
	    rs1 = u_sigaction(SIGCLD,&osh,nullptr) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigaction) */
	return (rs >= 0) ? pid : rs ;
}
/* end subroutine (uc_forkdetached) */



/* isproc SUPPORT */
/* lang=C++20 */

/* is a process (specified by PID) currently in the system? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	isproc

	Description:
	This subroutine checks to see if the specific process
	identified by its PID, is still on the system. Note that
	even zombie processes can satisfy the search requirement
	on some systems!

	Synopsis:
	int isproc(pid_t pid) noex

	Arguments:
	pid			PID of process to search for

	Returns:
	true			process was found on system
	false			process was not found on system
	<0			error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int isproc(pid_t pid) noex {
	int		rs ;
	int		f = false ;
	if ((rs = u_kill(pid,0)) >= 0) {
	    f = true ;
	} else if (rs == SR_PERM) {
	    f = true ;
	    rs = SR_OK ;
	} else if (rs == SR_SRCH) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (isproc) */



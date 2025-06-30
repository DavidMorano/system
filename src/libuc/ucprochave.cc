/* ucprochave SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* is a process (specified by PID) currently in the system? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ucprochave

	Description:
	This subroutine checks to see if the specific process
	identified by its PID, is still on the system.  Note that
	even zombie processes can satisfy the search requirement
	on some systems!

	Synopsis:
	int ucprochave(pid_t pid) noex

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

#include	"ucprochave.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int ucprochave(pid_t pid) noex {
	int		rs = SR_INVALID ;
	int		f = false ;
	if (pid >= 0) {
	    if ((rs = u_kill(pid,0)) >= 0) {
	        f = true ;
	    } else if (rs == SR_PERM) {
	        f = true ;
	        rs = SR_OK ;
	    } else if (rs == SR_SRCH) {
	        rs = SR_OK ;
	    }
	} /* end if (valid) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ucprochave) */



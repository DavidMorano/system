/* proginfo_real SUPPORT */
/* lang=C++20 */

/* proginfo-information real-UID functions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	------------------------------------------------------------------------
	Name:
	proginfo_realbegin

	Description:
	This subroutine is used to reset the UID, GID, or both so
	that the effective credentials of the program temporarily
	become those of the real user or real group.

	Synopsis:
	int proginfo_realbegin(proginfo *pip) noex

	Arguments:
	pip		program-information pointer

	Returns:
	>=0	length of PR
	<0	error (system-return)

	------------------------------------------------------------------------
	Name:
	proginfo_realend

	Synopsis:
	int proginfo_realend(proginfo *pip) noex

	Arguments:
	pip		program-information pointer

	Returns:
	>=0	length of PR
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"proginfo.hh"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int proginfo_realbegin(proginfo *pip) noex {
	int		rs = SR_FAULT ;
	if (pip) {
	    rs = SR_OK ;
	    if (pip->pf.setuid) {
	        const uid_t	euid = geteuid() ;
	        if (euid != pip->uid) u_seteuid(pip->uid) ;
	    }
	    if (pip->pf.setgid) {
	        const gid_t	egid = getegid() ;
	        if (egid != pip->gid) u_setegid(pip->gid) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (proginfo_realbegin) */

int proginfo_realend(proginfo *pip) noex {
	int		rs = SR_FAULT ;
	if (pip) {
	    rs = SR_OK ;
	    if (pip->pf.setgid) {
	        const gid_t	egid = getegid() ;
	        if (egid != pip->egid) u_setegid(pip->egid) ;
	    } /* end if (setgid) */
	    if (pip->pf.setuid) {
	        const uid_t	euid = geteuid() ;
	        if (euid != pip->euid) u_seteuid(pip->euid) ;
	    } /* end if (setuid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (proginfo_realend) */



/* getngroups */
/* lang=C20 */

/* get the maximum number of supplemetary groups allowed per process */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getngroups

	Description:
	Here we get the maximum number of supplemtary groups (GIDs)
	allowed per process.  The number is cached on the first
	fetch and then accessed from the cache on subsequent requests.
	Yes, we are multithread safe.  This number (maximum groups)
	cannot (usually) change without an intervening reboot of
	the system (thus it being completely acceptable to cache
	forever).

	Synopsis:
	int getngroups() noex

	Arguments:
	-

	Returns:
	<0		error (errno)
	==0		should not happen
	>0		number of suplemental groups allowed

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#define	GETNGROUPS	struct getngroups


/* external subroutines */


/* external variables */


/* local structures */

struct getngroups {
	int		n ;
} ;


/* forward references */


/* local variables */

static GETNGROUPS	getngroups_data ;


/* exported subroutines */

int getngroups() noex {
	GETNGROUPS	*gnp = &getngroups_data ;
	int		rs ;
	if ((rs = gnp->n) == 0) {
	    cint	cmd = _SC_NGROUPS_MAX ;
	    if ((rs = uc_confsys(cmd,nullptr)) >= 0) {
	        gnp->n = rs ;
	    }
	} /* end if (needed value) */
	return rs ;
}
/* end subroutine (getngroups) */



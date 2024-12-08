/* getngroups SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* get the maximum number of supplemetary groups allowed per process */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

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

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

struct groupmgr {
	int		n ;
} ;


/* forward references */


/* local variables */

static groupmgr		getngroups_data ;


/* exported variables */


/* exported subroutines */

int getngroups() noex {
	groupmgr	*gnp = &getngroups_data ;
	int		rs ;
	if (gnp->n == 0) {
	    cint	cmd = _SC_NGROUPS_MAX ;
	    if ((rs = uc_sysconf(cmd,nullptr)) >= 0) {
	        gnp->n = rs ;
	    }
	} else {
	    rs = gnp->n ;
	}
	return rs ;
}
/* end subroutine (getngroups) */



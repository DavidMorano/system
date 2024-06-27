/* getngroups SUPPORT */
/* lang=C++20 */

/* get the maximum number of supplemetary groups allowed per process */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

	= 2023-12-11,
	I switched to using C++11 |atomic|.

*/

/* Copyright © 1998,2023 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getngroups
	getngroupsx

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
	int getngroupsx(int cmd) noex

	Arguments:
	cmd		command for |getngroupsx| (0=get, 1=invalidate)

	Returns:
	<0		error (errno)
	==0		should not happen
	>0		number of suplemental groups allowed

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<atomic>
#include	<usystem.h>
#include	<localmisc.h>

#include	"getngroups.h"


/* local defines */


/* imported namespaces */

using std::atomic_int ;
using std::memory_order_relaxed ;


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct groupinfo {
	atomic_int	ng ;
	operator int () noex ;
	int operator () (int) noex ;
    } ; /* end struct (groupinfo) */
}


/* forward references */


/* local variables */

static groupinfo	getngroups_data ;


/* exported subroutines */

int getngroups() noex {
	return getngroups_data ;
}
/* end subroutine (getngroups) */

int getngroupsx(int x) noex {
	return getngroups_data(x) ;
}
/* end subroutine (getngroups) */


/* local subroutines */

groupinfo::operator int () noex {
	int		rs ;
	if ((rs = ng) == 0) {
	    cint	cmd = _SC_NGROUPS_MAX ;
	    if ((rs = uc_sysconfval(cmd,nullptr)) >= 0) {
	        ng = rs ;
	    }
	} /* end if (needed value) */
	return rs ;
}
/* end method (groupinfo::operator) */

int groupinfo::operator () (int x) noex {
	int		rs = SR_INVALID ;
	switch (x) {
	case 0:
	    rs = (*this) ;
	    break ;
	case 1:
	    rs = ng.exchange(0,memord_relaxed) ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end method (groupinfo::operator) */



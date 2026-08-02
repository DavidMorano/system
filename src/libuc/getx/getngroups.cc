/* getngroups SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the maximum number of supplemetary groups allowed per process */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

	= 2023-12-11, David A­D­ Morano
	I switched to using C++11 |atomic|.

*/

/* Copyright © 1998,2023 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getngroups
	getngroupsx

	Description:
	[getngroups] Here we get the maximum number of supplemtary
	groups (GIDs) allowed per process.  The number is cached
	on the first fetch and then accessed from the cache on
	subsequent requests.  Yes, we are multithread safe.  This
	number (maximum groups) cannot (usually) change without an
	intervening reboot of the system (thus it being completely
	acceptable to cache forever).
	[getngroupsx] The given value is a command:
		0	return value (cached or otherwise)
		1	invalidate any cached value

	Synopsis:
	int getngroups() noex
	int getngroupsx(grouptypes cmd) noex

	Arguments:
	cmd		command for |getngroupsx| (0=get, 1=invalidate)

	Returns:
	>0		number of suplemental groups allowed
	==0		should not happen
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<atomic>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucsysconf.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

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
    struct groupmgr {
	atomic_int	ng ;
	operator int () noex ;
	int operator () (grouptypes) noex ;
    } ; /* end struct (groupmgr) */
} /* end namespace */


/* forward references */


/* local variables */

static groupmgr		getngroups_data ;


/* exported variables */


/* exported subroutines */

int getngroups() noex {
	return getngroups_data ;
} /* end subroutine (getngroups) */

int getngroupsx(grouptypes x) noex {
	return getngroups_data(x) ;
} /* end subroutine (getngroups) */


/* local subroutines */

groupmgr::operator int () noex {
	int		rs ;
	if ((rs = ng) == 0) {
	    cint	cmd = _SC_NGROUPS_MAX ;
	    if ((rs = uc_sysconfval(cmd,nullptr)) >= 0) {
	        ng = rs ;
	    }
	} /* end if (needed value) */
	return rs ;
} /* end method (groupmgr::operator) */

int groupmgr::operator () (grouptypes x) noex {
	int		rs{} ;
	switch (x) {
	case grouptype_sys:
	    rs = (*this) ;
	    break ;
	case grouptype_cache:
	    rs = ng.exchange(0,memord_relaxed) ;
	    break ;
	default:
	    rs = SR_INVALID ;
	    break ;
	} /* end switch */
	return rs ;
} /* end method (groupmgr::operator) */



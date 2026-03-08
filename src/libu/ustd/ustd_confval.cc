/* ustd_confval SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* get system configuration information */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	std_confval

	Description:
	Retrieve values from the system.

	Synopsis:
    	int ustd_confval(int req,long *lp) noex

	Arguments:
	req		configuration value to request
	lp		optional pointer to longword to receive result

	Returns:
	>0		valid and the value is returned with this length
	0		valid but there was no value associated
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>		/* |confstr(2)| */
#include	<cerrno>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>		/* |intsat(3u)| */
#include	<sysconfcmds.h>		/* commands for |sysconf(3c)| */
#include	<localmisc.h>

#include	"ustd.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local bool hasnolimit(int) noex ;


/* local variables */

constexpr int		nolimits[] = {
	_SC_AIO_MAX,
	_SC_ATEXIT_MAX,
	_SC_MQ_OPEN_MAX,
	_SC_THREAD_THREADS_MAX,
	_SC_THREAD_KEYS_MAX,
	_SC_THREAD_DESTRUCTOR_ITERATIONS,
	-1
} ; /* end array (nolimits) */


/* exported variables */


/* exported subroutines */

namespace libu {
    int ustd_confval(int req,long *lp) noex {
	int		rs ;
	errno = 0 ;
	if (long res ; (res = sysconf(req)) >= 0L) {
	    if (lp) *lp = res ;
	    rs = intsat(res) ;
	} else {
	    if (errno) {
	        rs = (- errno) ;
	    } else {
		if (hasnolimit(req)) {
		    rs = INT_MAX ;
		    if (lp) *lp = INT_MAX ;
		} else {
		    rs = SR_NOTSUP ;
		}
	    } /* end if */
	} /* end if */
	return rs ;
    } /* end subroutine (ustd_confval) */
} /* end namespace (libu) */


/* local subroutines */

local bool hasnolimit(int name) noex {
    	bool f = false ;
	for (int i = 0 ; (! f) && (nolimits[i] >= 0) ; i += 1) {
	    f = (name == nolimits[i]) ;
	} /* end for */
	return f ;
} /* end subroutine (hasnolimit) */



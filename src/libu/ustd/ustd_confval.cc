/* ustd_confstr SUPPORT */
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
	std_constr

	Description:
	This subroutine is NOT the same as the UNIX®-System version.
	This subroutine returns "OVERFLOW" when the user supplied
	buffer is not big enough to hold the associated value.

	Synopsis:
	int ustd_confstr(char *rbuf,int rlen,int req) noex

	Arguments:
	rbuf		user supplied buffer to hold result
	rlen		length of user supplied value
	req		configuration value to request

	Returns:
	>0		valid and the value is returned with this length
	0		valid but there was no value associated
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>		/* |confstr(2)| */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<errtimer.hh>
#include	<sysconfcmds.h>
#include	<intsat.h>
#include	<localmisc.h>

#include	"ustd.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */


/* local subroutines */

namespace ustd {
    int ustd_confval(int req,long *lp) noex {
	int		rs ;
	errno = 0 ;
	if (long res ; (res = sysconf(req)) >= 0L) {
	    if (lp) *lp = res ;
	    rs = intsat(res) ;
	} else {
	    rs = (errno) ? (- errno) : SR_NOTSUP ;
	}
	return rs ;
    } /* end subroutine (ustd_confval) */
} /* end namespace (ustd) */



/* usysop SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	u_adjtime
	u_stime
	u_time

	Description:
	System functions.

	Synopses:
	int u_adjtime(CTIMEVAL *tvp,TIMEVAL *ovp) noex ;
	int u_stime(time_t *tp) noex ;
	int u_time(time_t *rp) noex ;

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/time.h>		/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* CSTD */
#include	<usysbase.h>		/* CSTD */
#include	<usyscalls.h>		/* CSTD */
#include	<usupport.h>		/* CSTD */
#include	<intsat.h>		/* CSTD */
#include	<localmisc.h>		/* CSTD */

#include	"usysop.h"

import usysconf ;

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

int u_adjtime(CTIMEVAL *tvp,TIMEVAL *ovp) noex {
	int		rs = SR_INVALID ;
	if (tvp || ovp) {
	    if ((rs = adjtime(tvp,ovp)) < 0) {
	        rs = (neg errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (u_adjtime) */

int u_stime(time_t *tp) noex {
	int		rs = SR_FAULT ;
	if (tp) {
	    if ((rs = stime(tp)) < 0) {
		rs = (neg errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (u_stime) */

int u_time(time_t *rp) noex {
	int		rs ;
	if (time_t ut ; (ut = time(rp)) < 0) {
	    rs = (neg errno) ;	/* really only can be |EFAULT| */
	} else {
	    rs = intsat(ut) ;
	}
	return rs ;
} /* end subroutine (u_time) */



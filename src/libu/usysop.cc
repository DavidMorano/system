/* usysop SUPPORT */
/* encoding=ISO8859-1 */
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

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/time.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<ctime>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<usupport.h>
#include	<intsat.h>
#include	<localmisc.h>

#include	"usysop.h"


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
	int		rs ;
	if ((rs = adjtime(tvp,ovp)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (u_adjtime) */

int u_stime(time_t *tp) noex {
	int		rs = SR_FAULT ;
	if (tp) {
	    if ((rs = stime(tp)) < 0) {
		rs = (- errno) ;
	    }
	}
	return rs ;
}
/* end subroutine (u_stime) */

int u_time(time_t *rp) noex {
	int		rs ;
	if (time_t ut ; (ut = time(rp)) < 0) {
	    rs = (- errno) ;	/* really only can be |EFAULT| */
	} else {
	    rs = intsat(ut) ;
	}
	return rs ;
}
/* end subroutine (u_time) */



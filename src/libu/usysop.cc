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
	u_sysconfval
	u_sysconfstr

	Description:
	System functions.

	Synopses:
	int u_adjtime(CTIMEVAL *tvp,TIMEVAL *ovp) noex ;
	int u_stime(time_t *tp) noex ;
	int u_time(time_t *rp) noex ;
	int u_sysconfval(int cmd,long *rp) noex
	int u_sysconfstr(int cmd,char *rbuf,int rlen) noex

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<sys/types.h>
#include	<sys/time.h>
#include	<cerrno>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<usupport.h>
#include	<intsat.h>
#include	<localmisc.h>

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
	        rs = (- errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_adjtime) */

int u_stime(time_t *tp) noex {
	int		rs = SR_FAULT ;
	if (tp) {
	    if ((rs = stime(tp)) < 0) {
		rs = (- errno) ;
	    }
	} /* end if (non-null) */
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

int u_sysconfval(int cmd,long *rp) noex {
    return usysconfval(cmd,rp) ;
}

int u_sysconfstr(int cmd,char *rbuf,int rlen) noex {
    return usysconfstr(cmd,rbuf,rlen) ;
}



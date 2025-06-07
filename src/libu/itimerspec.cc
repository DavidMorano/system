/* itimerspec SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* UNIX® ITIMERSPEC object initialization */
/* version %I% last-modified %G% */


/* revision history:

	= 2014-04-04, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	itimerspec_load

	Description:
	These subroutines manipulate ITIMERSPEC objects.

	Synopsis:
	int itimerspec_load(ITIMERSPEC *tsp,CTIMESPEC *vp,CTIMESPEC *ip) noex {

	Arguments:
	tsp		pointer to ITIMERSPEC
	vp		pointer to TIMESPEC for the "value"
	ip		pointer to TIMESPEC for the "interval"

	Returns:
	>=0		OK
	<0		error (system-return)

	Comments:
	typedef struct itimerspec {		
		struct timespec	it_interval;	
		struct timespec	it_value;	
	} itimerspec_t ;

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/time.h>		/* <- interval timers are here */
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usyscalls.h>
#include	<localmisc.h>

#include	"itimerspec.h"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int itimerspec_load(ITIMERSPEC *tsp,CTIMESPEC *valp,CTIMESPEC *ivp) noex {
	int		rs = SR_FAULT ;
	if (tsp) {
	    rs = memclear(tsp) ;
	    if (valp) {
	        tsp->it_value = *valp ;
	    }
	    if (ivp) {
	        tsp->it_interval = *ivp ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (itimerspec_load) */



/* itimerspec SUPPORT */
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
	int itimerspec_load(ITIMERSPEC *tsp,time_t sec,long nsec) noex

	Arguments:
	tsp		pointer to ITIMERSPEC
	sec		seconds
	nsec		nanoseconds

	Returns:
	>=0		OK
	<0		error (system-return)

	Comments:
	typedef struct itimerspec {		
		struct timespec	it_interval;	
		struct timespec	it_value;	
	} itimerspec_t ;

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<ctime>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>		/* <- for |memclear(3u)| */
#include	<localmisc.h>

#include	"itimerspec.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int itimerspec_load(ITIMERSPEC *tsp,TIMESPEC *valp,TIMESPEC *intp) noex {
	int		rs = SR_FAULT ;
	if (tsp) {
	    memclear(tsp) ;
	    if (valp) {
	        tsp->it_value = *valp ;
	    }
	    if (intp) {
	        tsp->it_interval = *intp ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (itimerspec_load) */



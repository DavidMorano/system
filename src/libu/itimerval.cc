/* itimerval SUPPORT */
/* lang=C++20 */

/* UNIX® ITIMERVAL object initialization */
/* version %I% last-modified %G% */


/* revision history:

	= 2014-04-04, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	itimerval_load

	Description:
	These subroutines manipulate ITIMERVAL objects.

	Synopsis:
	int itimerval_load(ITIMERVAL *tsp,time_t sec,long nsec) noex

	Arguments:
	tsp		pointer to ITIMERVAL
	sec		seconds
	nsec		nanoseconds

	Returns:
	>=0		OK
	<0		error (system-return)

	Comments:
	typedef struct itimerval {		
		struct timespec	it_interval;	
		struct timespec	it_value;	
	} itimerval_t ;

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<ctime>
#include	<usystem.h>
#include	<localmisc.h>

#include	"itimerval.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int itimerval_load(ITIMERVAL *tsp,CTIMEVAL *valp,CTIMEVAL *ivp) noex {
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
/* end subroutine (itimerval_load) */



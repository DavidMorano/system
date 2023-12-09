/* timespec */
/* lang=C99 */

/* UNIX® TIMESPEC object initialization */


/* revision history:

	= 2014-04-04, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	These subroutines manipulate TIMESPEC objects.

	Synopsis:
	int timespec_load(TIMESPEC *tsp,time_t sec,long nsec) noex

	Arguments:
	tsp		pointer to TIMESPEC
	sec		seconds
	nsec		nanoseconds

	Returns:
	<0		error
	>=0		OK

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<time.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#ifndef	INTBILLION
#define	INTBILLION	1000000000
#endif

#ifndef	TIMESPEC
#define	TIMESPEC	struct timespec
#endif


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int timespec_load(TIMESPEC *tsp,time_t sec,long nsec) noex {
	if (tsp == NULL) return SR_FAULT ;
	while (nsec >= INTBILLION) {
	    sec += 1 ;
	    nsec -= INTBILLION ;
	}
	tsp->tv_sec = sec ;
	tsp->tv_nsec = nsec ;
	return SR_OK ;
}
/* end subroutine (timespec_load) */



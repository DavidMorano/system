/* timespec SUPPORT */
/* lang=C++20 */

/* UNIX® TIMESPEC object initialization */
/* version %I% last-modified %G% */


/* revision history:

	= 2014-04-04, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	timespec_load

	Description:
	These subroutines manipulate TIMESPEC objects.

	Synopsis:
	int timespec_load(TIMESPEC *tsp,time_t sec,long nsec) noex

	Arguments:
	tsp		pointer to TIMESPEC
	sec		seconds
	nsec		nanoseconds

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<ctime>
#include	<usystem.h>
#include	<localmisc.h>

#include	"timespec.h"


/* local defines */

#ifndef	INTBILLION
#define	INTBILLION	1000000000
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int timespec_load(TIMESPEC *tsp,time_t sec,long nsec) noex {
	int		rs = SR_FAULT ;
	if (tsp) {
	    while (nsec >= INTBILLION) {
	        sec += 1 ;
	        nsec -= INTBILLION ;
	    } /* end while */
	    tsp->tv_sec = sec ;
	    tsp->tv_nsec = nsec ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (timespec_load) */



/* ucclock SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* system clock management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uc_clock{x}

	Description:
	The UNIX® System "clock" subroutines.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<cerrno>
#include	<ctime>
#include	<usystem.h>


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_clockset(clockid_t cid,const TIMESPEC *tsp) noex {
	int		rs = SR_FAULT ;
	if (tsp) {
	    if ((rs = clock_settime(cid,tsp)) < 0) {
		rs = (-errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_clockset) */

int uc_clockres(clockid_t cid,TIMESPEC *tsp) noex {
	int		rs = SR_FAULT ;
	if (tsp) {
	    if ((rs = clock_getres(cid,tsp)) < 0) {
		rs = (-errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_clockres) */

int uc_clockget(clockid_t cid,TIMESPEC *tsp) noex {
	int		rs = SR_FAULT ;
	if (tsp) {
	    if ((rs = clock_gettime(cid,tsp)) < 0) {
		rs = (-errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_clockget) */



/* ucclock SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

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



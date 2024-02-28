/* usys_gethrtime SUPPORT */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David D-A- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

	We defines some system (global) variables in this module.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/time.h>		/* <- the money shot */
#include	<cerrno>
#include	<ctime>			/* Darwin |clock_xx(3)| */
#include	<usysflag.h>
#include	<clanguage.h>

#include	"usys_gethrtime.h"


/* local defines */


/* local typedefs */


/* external variables */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr bool		f_sunos		= F_SUNOS ;
constexpr bool		f_darwin	= F_DARWIN ;
constexpr bool		f_linux		= F_LINUX ;

constexpr unsigned long	onebillion = 1000000000 ;


/* exported variables */


/* exported subroutines */

/* GETHRTIME begin */
#if	defined(SYSHAS_GETHRTIME) && (SYSHAS_GETHRTIME > 0)

/* nothing needed; def in 'time.h' */

#else /* defined(SYSHAS_GETHRTIME) && (SYSHAS_GETHRTIME > 0) */

hrtime_t gethrtime() noex {
	const clockid_t	cid = CLOCK_MONOTONIC_RAW ;
	hrtime_t	r = 0 ;
	if constexpr (f_darwin) {
	    r = clock_gettime_nsec_np(cid) ;
	} else {
	    struct timespec	ts ;
	    if (clock_gettime(cid,&ts) >= 0) {
		hrtime_t	t = ts.tv_sec ;
		r = ts.tv_nsec ;
		r += (t * onebillion) ;
	    }
	} /* end if-constexpr (f_darwin) */
	return r ;
}

#endif /* (!defined(SYSHAS_GETHRTIME)) || (SYSHAS_GETHRTIME == 0) */
/* GETHRTIME end */



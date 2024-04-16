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

	We defines some missing OS interfaces.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */

/* GETHRTIME begin */
#if	defined(SYSHAS_GETHRTIME) && (SYSHAS_GETHRTIME > 0)

/* nothing needed; def in 'time.h' */

#else /* defined(SYSHAS_GETHRTIME) && (SYSHAS_GETHRTIME > 0) */

#include	<sys/time.h>		/* <- the money shot */
#include	<cerrno>
#include	<ctime>			/* Darwin |clock_xx(3)| */
#include	<usysflag.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>

#include	"usys_gethrtime.h"


#if	defined(OSNAME_DARWIN) && (OSNAME_DARWIN > 0)

hrtime_t gethrtime() noex {
	const clockid_t	cid = CLOCK_MONOTONIC_RAW ;
	return clock_gettime_nsec_np(cid) ;
}

#elif	defined(OSNAME_LINUX) && (OSNAME_LINUX > 0)

constexpr unsigned long	onebillion = 1000000000 ;

hrtime_t gethrtime() noex {
	TIMESPEC	ts ;
	const clockid_t	cid = CLOCK_MONOTONIC_RAW ;
	hrtime_t	r = 0 ;
	if (clock_gettime(cid,&ts) >= 0) {
	    hrtime_t	t = ts.tv_sec ;
	    r = ts.tv_nsec ;
	    r += (t * onebillion) ;
	}
	return r ;
}

#else /* all other OSes */

hrtime_t gethrtime() noex {
	return 0 ;
}

#endif /* which OS */


#endif /* (!defined(SYSHAS_GETHRTIME)) || (SYSHAS_GETHRTIME == 0) */
/* GETHRTIME end */



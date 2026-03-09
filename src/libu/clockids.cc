/* clockids SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* provides identifiers for UINX® system clock IDs */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-13, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	clockids

	Description:
	This object simply provides the identifying values used in
	selecting a UNIX® system clock.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>

#include	"clockids.hh"


/* local defines */

enum clockidmisses {
    clockidmiss_realtime = 1000,
    clockidmiss_monotonic,
    clockidmiss_monotonicraw,
    clockidmiss_monotonicrawappr,
    clockidmiss_cpuprocess,
    clockidmiss_cputhread,
    clockidmiss_boot,
    clockidmiss_bootappr,
    clockidmiss_overlast
} ; /* end enum (clockidmisses) */

#ifndef	CLOCK_REALTIME
#define	CLOCK_REALTIME			clockidmiss_realtime
#endif

#ifndef	CLOCK_MONOTONIC
#define	CLOCK_MONOTONIC			clockidmiss_monotonic
#endif

#ifndef	CLOCK_MONOTONIC_RAW
#define	CLOCK_MONOTONIC_RAW		clockidmiss_monotonicraw
#endif

#ifndef	CLOCK_MONOTONIC_RAW_APPROX
#define	CLOCK_MONOTONIC_RAW_APPROX	clockidmiss_monotonicrawappr
#endif

#ifndef	CLOCK_PROCESS_CPUTIME_ID
#define	CLOCK_PROCESS_CPUTIME_ID	clockidmiss_cpuprocess
#endif

#ifndef	CLOCK_THREAD_CPUTIME_ID
#define	CLOCK_THREAD_CPUTIME_ID		clockidmiss_cputhread
#endif

#ifndef	CLOCK_UPTIME_RAW
#define	CLOCK_UPTIME_RAW		clockidmiss_boot
#endif

#ifndef	CLOCK_UPTIME_RAW_APPROX
#define	CLOCK_UPTIME_RAW_APPROX		clockidmiss_bootappr
#endif


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */

const clockid_t	clockids::real			= CLOCK_REALTIME ;
const clockid_t	clockids::montonic		= CLOCK_MONOTONIC ;
const clockid_t	clockids::montonicraw		= CLOCK_MONOTONIC_RAW ;
const clockid_t	clockids::montonicrawappr	= CLOCK_MONOTONIC_RAW_APPROX ;
const clockid_t	clockids::cpuprocess		= CLOCK_PROCESS_CPUTIME_ID ;
const clockid_t	clockids::cputhread		= CLOCK_THREAD_CPUTIME_ID ;
const clockid_t	clockids::boot			= CLOCK_UPTIME_RAW ;
const clockid_t	clockids::bootappr		= CLOCK_UPTIME_RAW_APPROX ;

const clockids		clockid ;


/* exported subroutines */


/* local subroutines */



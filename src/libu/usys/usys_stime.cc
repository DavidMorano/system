/* usys_stime SUPPORT */
/* encoding=ISO8859-1 */
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

	Important note:
	It looks like only SunOS (really Solaris®) and Illumos®
	have the |stime(2)| system call.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */

/* STIME start */
#if	defined(SYSHAS_STIME) && (SYSHAS_STIME > 0)

#include	<unistd.h>		/* for SunOS */

#else /* defined(SYSHAS_STIME) && (SYSHAS_STIME > 0) */

#include	<sys/time.h>		/* type |timeval| */
#include	<cerrno>
#include	<ctime>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>

#include	"usys_stime.h"

unixret_t stime(const time_t *tp) noex {
	unixret_t	rc = -1 ;
	if (tp) {
	    TIMEVAL	tv{} ;
	    tv.tv_sec = *tp ;
	    rc = settimeofday(&tv,nullptr) ;
	} else {
	    errno = EFAULT ;
	} /* end if (non-null) */
	return rc ;
}

#endif /* (!defined(SYSHAS_STIME)) || (SYSHAS_STIME == 0) */
/* STIME end */



/* timespec HEADER */
/* lang=C99 */

/* time-spec object methods */


/* revision history:

	= 2014-04-04, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 2014 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Methods for the TIMESPEC object.


*******************************************************************************/

#ifndef	TIMESPEC_INCLUDE
#define	TIMESPEC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>


#ifndef	TIMESPEC
#define	TIMESPEC	struct timespec
#endif

#ifdef	__cplusplus
extern "C" {
#endif

extern int timespec_load(TIMESPEC *,time_t,long) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* TIMESPEC_INCLUDE */



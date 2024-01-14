/* itimerspec HEADER */
/* lang=C99 */

/* interval-timer object methods */


/* revision history:

	= 2014-04-04, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Methods for the ITIMERSPEC object.

*******************************************************************************/

#ifndef	ITIMERSPEC_INCLUDE
#define	ITIMERSPEC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<time.h>
#include	<timespec.h>
#include	<localmisc.h>


#ifndef	ITIMERSPEC
#define	ITIMERSPEC	struct itimerspec
#endif

EXTERNC_begin

extern int itimerspec_load(ITIMERSPEC *,TIMESPEC *,TIMESPEC *) ;

EXTERNC_end


#endif /* ITIMERSPEC_INCLUDE */



/* itimerspec HEADER */
/* charset=ISO8859-1 */
/* lang=C99 */

/* interval-timer object methods */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	ITIMERSPEC

  	Description:
	Methods for the ITIMERSPEC object.

*******************************************************************************/

#ifndef	ITIMERSPEC_INCLUDE
#define	ITIMERSPEC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/time.h>		/* <- interval timers are here */
#include	<time.h>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<timespec.h>


EXTERNC_begin

extern int	itimerspec_load(ITIMERSPEC *,CTIMESPEC *,CTIMESPEC *) noex ;

EXTERNC_end


#endif /* ITIMERSPEC_INCLUDE */



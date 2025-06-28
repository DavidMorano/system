/* itimerspec HEADER */
/* charset=ISO8859-1 */
/* lang=C99 */

/* interval-timer object methods */
/* version %I% last-modified %G% */


/* revision history:

	= 2014-04-04, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

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
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<timespec.h>


EXTERNC_begin

extern int	itimerspec_load(ITIMERSPEC *,CTIMESPEC *,CTIMESPEC *) noex ;

EXTERNC_end


#endif /* ITIMERSPEC_INCLUDE */



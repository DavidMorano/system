/* timespec HEADER */
/* charset=ISO8859-1 */
/* lang=C99 */

/* time-spec object methods */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	timespec

	Description:
	Methods for the TIMESPEC object.

*******************************************************************************/

#ifndef	TIMESPEC_INCLUDE
#define	TIMESPEC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	timespec_load(TIMESPEC *,time_t,long) noex ;

EXTERNC_end


#endif /* TIMESPEC_INCLUDE */



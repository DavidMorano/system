/* sigevent HEADER */
/* lang=C99 */

/* signal event object methods */
/* version %I% last-modified %G% */


/* revision history:

	= 2014-04-04, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Methods for the signal-event object.

*******************************************************************************/

#ifndef	SIGEVENT_INCLUDE
#define	SIGEVENT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<signal.h>
#include	<localmisc.h>


#ifndef	SIGEVENT
#define	SIGEVENT	struct sigevent
#endif


#ifdef	__cplusplus
extern "C" {
#endif

extern int sigevent_load(SIGEVENT *,int,int,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* SIGEVENT_INCLUDE */



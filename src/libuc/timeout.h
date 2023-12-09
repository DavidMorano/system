/* timeout INCLUDE */
/* lang=C99 */

/* time-out interface */
/* version %I% last-modified %G% */


/* revision history:

	= 2014-04-04, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This file provides the definition of the TIMEOUT object, and its
	method declarations.


*******************************************************************************/

#ifndef	TIMEOUT_INCLUDE
#define	TIMEOUT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<localmisc.h>


#define	TIMEOUT		struct timeout_entry


enum timeoutcmds {
	timeoutcmd_set,
	timeoutcmd_cancel,
	timeoutcmd_overlast
} ;

struct timeout_entry {
	void		*objp ;
	void		*metp ;
	time_t		val ;
	uint		tag ;
	int		arg ;
	int		id ; /* created by the system */
} ;

typedef struct timeout_entry	timeout ;

#ifdef	__cplusplus
extern "C" {
#endif

typedef int	(*timeout_met)(void *,uint,int) noex ;

extern int timeout_load(TIMEOUT *,time_t,void *,timeout_met,uint,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* TIMEOUT_INCLUDE */



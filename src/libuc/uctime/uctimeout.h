/* uctimeout HEADER */
/* charset=ISO8859-1 */
/* lang=C99 */

/* time-out interface */
/* version %I% last-modified %G% */


/* revision history:

	= 2014-04-04, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	timeout

	Description:
	This file provides the definition of the TIMEOUT object,
	and its method declarations.

*******************************************************************************/

#ifndef	UCTIMEOUT_INCLUDE
#define	UCTIMEOUT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<time.h>		/* CSTD |time_t| */
#include	<clanguage.h>		/* CSTD */
#include	<usysbase.h>		/* CSTD */


#define	TIMEOUT		struct timeout_entry


EXTERNC_begin

typedef int (*timeout_f)(void *,uint,int) noex ;

EXTERNC_end

enum timeoutcmds {
	timeoutcmd_set,
	timeoutcmd_cancel,
	timeoutcmd_overlast
} ; /* end enum (timeoutcmds) */

struct timeout_entry {
	timeout_f	metf ;		/* method function (C-linkage) */
	void		*objp ;		/* object pointer */
	time_t		val ;
	uint		tag ;
	int		arg ;
	int		id ;		/* created by the system */
} ; /* end (struct timeout_entry) */

typedef TIMEOUT		timeout ;

EXTERNC_begin

extern int timeout_load(timeout *,time_t,void *,timeout_f,uint,int) noex ;

extern int uc_timeout(int,timeout *) noex ;

EXTERNC_end


#endif /* UCTIMEOUT_INCLUDE */



/* dispatcher HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* dispatcher object */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-03-01, David A-D- Morano
	This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	dispatcher

	Description:
	This object is work dispatcher.  It dispatches work jobs
	to a pool of threads (created and managed by this object).

*******************************************************************************/

#ifndef	DISPATCHER_INCLUDE
#define	DISPATCHER_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ciq.h>			/* LIBUC */
#include	<psem.h>		/* LIBUC */
#include	<vecobj.h>		/* LIBUC */


#define	DISPATCHER	struct dispatcher_head


struct dispatcher_head {
	ciq		wq ;		/* work Q */
	psem		ws ;		/* work semaphore */
	vecobj		tids ;
	void		*callsub ;	/* called subroutine entry-address */
	void		*callarg ;	/* called subroutine argument */
	uint		magval ;
	volatile int	f_exit ;	/* CMD to exit immediately */
	volatile int	f_done ;	/* CMD to exit after work completed */
	int		nthr ;		/* concurrency */
} ; /* end struct */

typedef	DISPATCHER	dispatcher ;

EXTERNC_begin

extern int	dispatcher_start(dispatcher *,int,void *,void *) noex ;
extern int	dispatcher_add(dispatcher *,void *) noex ;
extern int	dispatcher_finish(dispatcher *,int) noex ;

EXTERNC_end


#endif /* DISPATCHER_INCLUDE */



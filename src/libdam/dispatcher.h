/* dispatcher */

/* dispatcher object */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object is work dispatcher.  It dispatches work jobs to a pool
	of threads (created and managed by this object).


*******************************************************************************/


#ifndef	DISPATCHER_INCLUDE
#define	DISPATCHER_INCLUDE	1


#include	<envstandards.h>	/* must be before others */

#include	<sys/types.h>
#include	<sys/param.h>
#include	<limits.h>

#include	<usystem.h>
#include	<ptm.h>
#include	<ptc.h>
#include	<ciq.h>
#include	<psem.h>
#include	<vecobj.h>
#include	<localmisc.h>


#define	DISPATCHER	struct dispatcher_head


struct dispatcher_head {
	uint		magic ;
	CIQ		wq ;		/* work Q */
	PSEM		ws ;		/* work semaphore */
	vecobj		tids ;
	void		*callsub ;	/* called subroutine entry-address */
	void		*callarg ;	/* called subroutine argument */
	DISPATCHER_THR	*threads ;
	volatile int	f_exit ;	/* CMD to exit immediately */
	volatile int	f_done ;	/* CMD to exit after work completed */
	int		nthr ;		/* concurrency */
} ;


#if	(! defined(DISPATCHER_MASTER)) || (DISPATCHER_MASTER == 0)

#ifdef	__cplusplus
extern "C" {
#endif

extern int	dispatcher_start(DISPATCHER *,int,void *,void *) ;
extern int	dispatcher_add(DISPATCHER *,void *) ;
extern int	dispatcher_finish(DISPATCHER *,int) ;

#ifdef	__cplusplus
}
#endif

#endif /* DISPATCHER_MASTER */

#endif /* DISPATCHER_INCLUDE */



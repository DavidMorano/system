/* csem INCLUDE */
/* lang=C20 */

/* Counting-Semaphore (CSEM) */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-28, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright � 1999 David A�D� Morano.  All rights reserved. */

#ifndef	CSEM_INCLUDE
#define	CSEM_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<pthread.h>
#include	<ptm.h>
#include	<ptc.h>
#include	<localmisc.h>
#include	<clanguage.h>


#define	CSEM		struct csem_head
#define	CSEM_MAGIC	0x26293175


struct csem_head {
	ptm		*mxp ;
	ptc		*cvp ;
	uint		magic ;
	int		count ;		/* this is the real data! */
	int		waiters ;	/* this is some extra */
} ;

typedef CSEM		csem ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int	csem_create(csem *,int,int) noex ;
extern int	csem_decr(csem *,int,int) noex ;
extern int	csem_incr(csem *,int) noex ;
extern int	csem_count(csem *) noex ;
extern int	csem_waiters(csem *) noex ;
extern int	csem_destroy(csem *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* CSEM_INCLUDE */



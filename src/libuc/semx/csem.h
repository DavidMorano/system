/* csem HEADER (Counting Semaphore) */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* Counting-Semaphore (CSEM) */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-28, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	CSEM_INCLUDE
#define	CSEM_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<pthread.h>
#include	<ptm.h>
#include	<ptc.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	CSEM		struct csem_head
#define	CSEM_MAGIC	0x26293175


struct csem_head {
	ptm		*mxp ;
	ptc		*cvp ;
	uint		magic ;
	int		cnt ;		/* this is the real data! */
	int		nwaiting ;	/* number waiting */
} ;

#ifdef	__cplusplus
enum csemmems {
	csemmem_count,
	csemmem_waiters,
	csemmem_destroy,
	csemmem_overlast
} ;
struct csem ;
struct csem_co {
	csem		*op = nullptr ;
	int		w = -1 ;
	void operator () (csem *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (csem_co) */
struct csem : csem_head {
	csem_co		count ;
	csem_co		waiters ;
	csem_co		destroy ;
	csem() noex {
	    count(this,csemmem_count) ;
	    waiters(this,csemmem_waiters) ;
	    destroy(this,csemmem_destroy) ;
	} ;
	csem(const csem &) = delete ;
	csem &operator = (const csem &) = delete ;
	int create(int = 0,int = 0) noex ;
	int decr(int = 1,int = -1) noex ;
	int incr(int = 1) noex ;
	operator int () noex ;
	void dtor() noex ;
	~csem() {
	    dtor() ;
	} ;
} ; /* end struct (csem) */
#else	/* __cplusplus */
typedef CSEM		csem ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	csem_create(csem *,int,int) noex ;
extern int	csem_decr(csem *,int,int) noex ;
extern int	csem_incr(csem *,int) noex ;
extern int	csem_count(csem *) noex ;
extern int	csem_waiters(csem *) noex ;
extern int	csem_destroy(csem *) noex ;

EXTERNC_end


#endif /* CSEM_INCLUDE */



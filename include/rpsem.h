/* rpsem HEADER ("real" POSIX® Semaphore) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* POSIX© unnamed Semaphore (RPSEM) */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-28, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	rpsem

	Description:
	These are (real) "unnamed" POSIX® semaphores.

*******************************************************************************/

#ifndef	RPSEM_INCLUDE
#define	RPSEM_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<semaphore.h>		/* POSIX® semaphores */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	RPSEM		struct rpsem_head

struct rpsem_head {
	sem_t		ps ;
} ;

#ifdef	__cplusplus
enum rpsemmems {
	rpsemmem_wait,
	rpsemmem_waiter,
	rpsemmem_trywait,
	rpsemmem_post,
	rpsemmem_count,
    	rpsemmem_destroy,
	rpsemmem_overlast
} ;
struct rpsem ;
struct rpsem_co {
	rpsem		*op = nullptr ;
	int		w = -1 ;
	void operator () (rpsem *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (int = -1) noex ;
	operator int () noex {
	    return operator () () ;
	} ;
} ; /* end struct (rpsem_co) */
struct rpsem : rpsem_head {
	rpsem_co		wait ;
	rpsem_co		waiter ;
	rpsem_co		trywait ;
	rpsem_co		post ;
	rpsem_co		count ;
	rpsem_co		destroy ;
	rpsem() noex {
	    wait(this,rpsemmem_wait) ;
	    waiter(this,rpsemmem_waiter) ;
	    trywait(this,rpsemmem_trywait) ;
	    post(this,rpsemmem_post) ;
	    count(this,rpsemmem_count) ;
	    destroy(this,rpsemmem_destroy) ;
	} ;
	rpsem(const rpsem &) = delete ;
	rpsem &operator = (const rpsem &) = delete ;
	int create(int = 0,int = -1) noex ;
	operator int () noex ;
	void dtor() noex ;
	destruct rpsem() {
	    dtor() ;
	} ;
} ; /* end struct (rpsem) */
#else	/* __cplusplus */
typedef RPSEM		rpsem ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	rpsem_create(rpsem *,int,int) noex ;
extern int	rpsem_destroy(rpsem *) noex ;
extern int	rpsem_wait(rpsem *) noex ;
extern int	rpsem_waiter(rpsem *,int) noex ;
extern int	rpsem_trywait(rpsem *) noex ;
extern int	rpsem_post(rpsem *) noex ;
extern int	rpsem_count(rpsem *) noex ;

EXTERNC_end


#endif /* RPSEM_INCLUDE */



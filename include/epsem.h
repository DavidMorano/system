/* epsem HEADER (emulated POSIX® Semaphore) */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* unnamed POSIX© Semaphore (EPSEM) */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-28, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	epsem

	Description:
	These are "unnamed" POSIX® semaphores.  This module implements
	an emulated POSIX® semaphore.  I have to write a look-alike
	implementation of POSIX semaphores myself because the Apple
	Darwin implemention does not allow for the un-named semaphore
	use case!  Yes, once and (and again, and again) the Apple
	Darwin UNIX-like implementation sucks cock meat.  F*ck Apple
	Darwin.

*******************************************************************************/

#ifndef	EPSEM_INCLUDE
#define	EPSEM_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<csem.h>		/* <- counting semaphore (DAM) */


#define	EPSEM		csem		/* <- the "money" shot */

#ifdef	__cplusplus
enum epsemmems {
	epsemmem_wait,
	epsemmem_waiter,
	epsemmem_trywait,
	epsemmem_post,
	epsemmem_count,
    	epsemmem_destroy,
	epsemmem_overlast
} ;
struct epsem ;
struct epsem_co {
	epsem		*op = nullptr ;
	int		w = -1 ;
	void operator () (epsem *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (int = -1) noex ;
	operator int () noex {
	    return operator () () ;
	} ;
} ; /* end struct (epsem_co) */
struct epsem : csem {
	epsem_co		wait ;
	epsem_co		waiter ;
	epsem_co		trywait ;
	epsem_co		post ;
	epsem_co		count ;
	epsem_co		destroy ;
	epsem() noex {
	    wait(this,epsemmem_wait) ;
	    waiter(this,epsemmem_waiter) ;
	    trywait(this,epsemmem_trywait) ;
	    post(this,epsemmem_post) ;
	    count(this,epsemmem_count) ;
	    destroy(this,epsemmem_destroy) ;
	} ;
	epsem(const epsem &) = delete ;
	epsem &operator = (const epsem &) = delete ;
	int create(int = 0,int = -1) noex ;
	operator int () noex ;
	void dtor() noex ;
	destruct epsem() {
	    dtor() ;
	} ;
} ; /* end struct (epsem) */
#else	/* __cplusplus */
typedef EPSEM		epsem ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	epsem_create(epsem *,int,int) noex ;
extern int	epsem_destroy(epsem *) noex ;
extern int	epsem_wait(epsem *) noex ;
extern int	epsem_waiter(epsem *,int) noex ;
extern int	epsem_trywait(epsem *) noex ;
extern int	epsem_post(epsem *) noex ;
extern int	epsem_count(epsem *) noex ;

EXTERNC_end


#endif /* EPSEM_INCLUDE */



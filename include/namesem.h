/* namesem HEADER (named semaphore) */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* UNIX® -- named -- Counting Semaphore (NAMESEM) */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	NAMESEM_INCLUDE
#define	NAMESEM_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* |mode_t| */
#include	<semaphore.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	NAMESEM_MAGIC	0x31419877
#define	NAMESEM		struct namesem_head


struct namesem_head {
	sem_t		*sp ;
	cchar		*name ;
	uint		magic ;
} ;

#ifdef	__cplusplus
enum namesemmems {
	namesemmem_close,
	namesemmem_wait,
	namesemmem_waiti,
	namesemmem_waiter,
	namesemmem_trywait,
	namesemmem_post,
	namesemmem_unlink,
	namesemmem_count
} ;
struct namesem ;
struct namesem_co {
	namesem		*op = nullptr ;
	int		w = -1 ;
	void operator () (namesem *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (int = -1) noex ;
	operator int () noex {
	    return operator () () ;
	} ;
} ; /* end struct (namesem_co) */
struct namesem : namesem_head {
	namesem_co	close ;
	namesem_co	wait ;
	namesem_co	waiti ;
	namesem_co	waiter ;
	namesem_co	trywait ;
	namesem_co	post ;
	namesem_co	unlink ;
	namesem_co	count ;
	namesem() noex {
	    close(this,namesemmem_close) ;
	    wait(this,namesemmem_wait) ;
	    waiti(this,namesemmem_waiti) ;
	    waiter(this,namesemmem_waiter) ;
	    trywait(this,namesemmem_trywait) ;
	    post(this,namesemmem_post) ;
	    unlink(this,namesemmem_unlink) ;
	    count(this,namesemmem_count) ;
	} ;
	namesem(const namesem &) = delete ;
	namesem &operator = (const namesem &) = delete ;
	int open(cchar *,int,mode_t,uint) noex ;
	operator int () noex ;
	void dtor() noex ;
	~namesem() {
	    dtor() ;
	} ;
} ; /* end struct (namesem) */
#else	/* __cplusplus */
typedef NAMESEM		namesem ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	namesem_open(namesem *,cchar *,int,mode_t,uint) noex ;
extern int	namesem_close(namesem *) noex ;
extern int	namesem_wait(namesem *) noex ;
extern int	namesem_waiti(namesem *) noex ;
extern int	namesem_waiter(namesem *,int) noex ;
extern int	namesem_trywait(namesem *) noex ;
extern int	namesem_post(namesem *) noex ;
extern int	namesem_unlink(namesem *) noex ;
extern int	namesem_count(namesem *) noex ;

extern int	namesemunlink(cchar *) noex ;
extern int	unlinknamesem(cchar *) noex ;

EXTERNC_end


#endif /* NAMESEM_INCLUDE */



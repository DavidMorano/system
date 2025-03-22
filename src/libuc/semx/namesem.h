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


#define	NAMESEM_MAGIC	0x31419877
#define	NAMESEM		struct namesem_head


struct namesem_head {
	sem_t		*sp ;
	cchar		*name ;
	uint		magic ;
} ;

typedef NAMESEM		namesem ;

EXTERNC_begin

extern int	namesem_open(namesem *,cchar *,int,mode_t,uint) noex ;
extern int	namesem_close(namesem *) noex ;
extern int	namesem_wait(namesem *) noex ;
extern int	namesem_waiti(namesem *) noex ;
extern int	namesem_trywait(namesem *) noex ;
extern int	namesem_post(namesem *) noex ;
extern int	namesem_unlink(namesem *) noex ;
extern int	namesem_count(namesem *) noex ;

extern int	namesemunlink(cchar *) noex ;
extern int	unlinknamesem(cchar *) noex ;

EXTERNC_end


#endif /* NAMESEM_INCLUDE */



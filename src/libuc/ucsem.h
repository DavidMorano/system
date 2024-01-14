/* ucsem HEADER */
/* lang=C20 */

/* UNIX® Counting Semaphore (UCSEM) */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCSEM_INCLUDE
#define	UCSEM_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<semaphore.h>
#include	<localmisc.h>


#define	UCSEM_MAGIC	0x31419877
#define	UCSEM		struct ucsem_head


struct ucsem_head {
	uint		magic ;
	sem_t		*sp ;
	cchar		*name ;
} ;

typedef UCSEM		ucsem ;

EXTERNC_begin

extern int	ucsem_open(ucsem *,cchar *,int,mode_t,uint) noex ;
extern int	ucsem_close(ucsem *) noex ;
extern int	ucsem_wait(ucsem *) noex ;
extern int	ucsem_waiti(ucsem *) noex ;
extern int	ucsem_trywait(ucsem *) noex ;
extern int	ucsem_post(ucsem *) noex ;
extern int	ucsem_unlink(ucsem *) noex ;

extern int	ucsemunlink(const char *) noex ;
extern int	unlinkucsem(const char *) noex ;

EXTERNC_end


#endif /* UCSEM_INCLUDE */



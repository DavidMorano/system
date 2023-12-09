/* ucsem INCLUDE */
/* lang=C20 */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCSEM_INCLUDE
#define	UCSEM_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<semaphore.h>
#include	<localmisc.h>


#define	UCSEM_MAGIC	0x31419877
#define	UCSEM		struct ucsem


struct ucsem {
	uint		magic ;
	sem_t		s, *sp ;
	cchar		*name ;
} ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int	ucsem_open(UCSEM *,cchar *,int,mode_t,uint) noex ;
extern int	ucsem_close(UCSEM *) noex ;
extern int	ucsem_wait(UCSEM *) noex ;
extern int	ucsem_waiti(UCSEM *) noex ;
extern int	ucsem_trywait(UCSEM *) noex ;
extern int	ucsem_post(UCSEM *) noex ;
extern int	ucsem_unlink(UCSEM *) noex ;

extern int	ucsemunlink(const char *) noex ;
extern int	unlinkucsem(const char *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UCSEM_INCLUDE */



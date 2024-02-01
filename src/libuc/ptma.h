/* ptma HEADER */
/* lang=C20 */

/* POSIX® Thread Mutex Attribute manipulation */
/* version %I% last-modified %G% */


/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	PTMA_INCLUDE
#define	PTMA_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<pthread.h>
#include	<clanguage.h>


#define	PTMA		pthread_mutexattr_t


#ifndef	TYPEDEF_PTMA
#define	TYPEDEF_PTMA
typedef PTMA		ptma ;
#endif

EXTERNC_begin

extern int	ptma_create(ptma *) noex ;
extern int	ptma_destroy(ptma *) noex ;
extern int	ptma_getprioceiling(ptma *,int *) noex ;
extern int	ptma_setprioceiling(ptma *,int) noex ;
extern int	ptma_getprotocol(ptma *,int *) noex ;
extern int	ptma_setprotocol(ptma *,int) noex ;
extern int	ptma_getpshared(ptma *,int *) noex ;
extern int	ptma_setpshared(ptma *,int) noex ;
extern int	ptma_getrobustnp(ptma *,int *) noex ;
extern int	ptma_setrobustnp(ptma *,int) noex ;
extern int	ptma_gettype(ptma *,int *) noex ;
extern int	ptma_settype(ptma *,int) noex ;

EXTERNC_end


#endif /* PTMA_INCLUDE */



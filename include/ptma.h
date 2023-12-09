/* ptma INCLUDE */
/* lang=C20 */

/* POSIX® Thread Mutex Attribute manipulation */
/* version %I% last-modified %G% */


/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	PTMA_INCLUDE
#define	PTMA_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<pthread.h>
#include	<clanguage.h>

#define	PTMA		pthread_mutexattr_t

#ifndef	TYPEDEF_PTMA
#define	TYPEDEF_PTMA
typedef PTMA		ptma ;
#endif

#ifdef	__cplusplus
extern "C" {
#endif

extern int	ptma_create(PTMA *) noex ;
extern int	ptma_destroy(PTMA *) noex ;
extern int	ptma_getprioceiling(PTMA *,int *) noex ;
extern int	ptma_setprioceiling(PTMA *,int) noex ;
extern int	ptma_getprotocol(PTMA *,int *) noex ;
extern int	ptma_setprotocol(PTMA *,int) noex ;
extern int	ptma_getpshared(PTMA *,int *) noex ;
extern int	ptma_setpshared(PTMA *,int) noex ;
extern int	ptma_getrobustnp(PTMA *,int *) noex ;
extern int	ptma_setrobustnp(PTMA *,int) noex ;
extern int	ptma_gettype(PTMA *,int *) noex ;
extern int	ptma_settype(PTMA *,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* PTMA_INCLUDE */



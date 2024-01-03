/* ptrwa HEADER */
/* lang=C20 */

/* POSIX® Thread read-write lock attribute manipulation */
/* version %I% last-modified %G% */


/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	PTRWA_INCLUDE
#define	PTRWA_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<pthread.h>
#include	<clanguage.h>

#define	PTRWA		pthread_rwlockattr_t

#ifdef	TYPEDEF_PTRWA
#define	TYPEDEF_PTRWA
typedef	PTRWA		ptrwa ;
#endif

#ifdef	__cplusplus
extern "C" {
#endif

extern int	ptrwa_create(PTRWA *) noex ;
extern int	ptrwa_destroy(PTRWA *) noex ;
extern int	ptrwa_setpshared(PTRWA *,int) noex ;
extern int	ptrwa_getpshared(PTRWA *,int *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* PTRWA_INCLUDE */



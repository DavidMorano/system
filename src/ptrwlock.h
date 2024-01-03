/* ptrwlock HEADER */
/* lang=C20 */

/* Posix Semaphore (PTRWLOCK) */
/* broken on SOLARIS! */
/* version %I% last-modified %G% */


/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	PTRWLOCK_INCLUDE
#define	PTRWLOCK_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<pthread.h>
#include	<clanguage.h>

#include	"ptrwa.h"

#define	PTRWLOCK		pthread_rwlock_t
#define	PTRWLOCK_NAMELEN	(MAXNAMELEN+1)

#ifdef	__cplusplus
extern "C" {
#endif

extern int	ptrwlock_create(PTRWLOCK *,PTRWA *) noex ;
extern int	ptrwlock_rdlock(PTRWLOCK *) noex ;
extern int	ptrwlock_tryrdlock(PTRWLOCK *) noex ;
extern int	ptrwlock_rdlockto(PTRWLOCK *,int) noex ;
extern int	ptrwlock_wrlock(PTRWLOCK *) noex ;
extern int	ptrwlock_trywrlock(PTRWLOCK *) noex ;
extern int	ptrwlock_wrlockto(PTRWLOCK *,int) noex ;
extern int	ptrwlock_unlock(PTRWLOCK *) noex ;
extern int	ptrwlock_destroy(PTRWLOCK *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* PTRWLOCK_INCLUDE */



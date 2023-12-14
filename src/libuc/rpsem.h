/* psem INCLUDE */
/* lang=C20 */

/* POSIX© unnamed Semaphore (PSEM) */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-28, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/*******************************************************************************

	These are POSIX semaphores.

*******************************************************************************/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	PSEM_INCLUDE
#define	PSEM_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<semaphore.h>
#include	<utypedefs.h>
#include	<clanguage.h>

#define	PSEM		sem_t

#ifndef	TYPEDEF_PSEM
#define	TYPEDEF_PSEM
typedef PSEM		psem ;
#endif

#ifdef	__cplusplus
extern "C" {
#endif

extern int	psem_create(psem *,int,int) noex ;
extern int	psem_destroy(psem *) noex ;
extern int	psem_wait(psem *) noex ;
extern int	psem_waiter(psem *,int) noex ;
extern int	psem_trywait(psem *) noex ;
extern int	psem_post(psem *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* PSEM_INCLUDE */



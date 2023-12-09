/* nsem INCLUDE */
/* lang=C20 */


/* revision history:

	= 1999-07-28, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/*******************************************************************************

	These are POSIX semaphores.

*******************************************************************************/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	NSEM_INCLUDE
#define	NSEM_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<semaphore.h>		/* <- POSIX semaphore */
#include	<clanguage.h>

#define	NSEM		sem_t
#define	NSEM_NAMELEN	14

#ifndef	TYPEDEF_NSEM
#define	TYPEDEF_NSEM
typedef NSEM		nsem ;
#endif

#ifdef	__cplusplus
extern "C" {
#endif

extern int	uc_opennsem(cchar *,int,mode_t,uint,NSEM **) noex ;

extern int	nsem_close(NSEM *) noex ;
extern int	nsem_count(NSEM *) noex ;
extern int	nsem_getvalue(NSEM *,int *) noex ;
extern int	nsem_wait(NSEM *) noex ;
extern int	nsem_waiter(NSEM *,int) noex ;
extern int	nsem_waiti(NSEM *) noex ;
extern int	nsem_trywait(NSEM *) noex ;
extern int	nsem_post(NSEM *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* NSEM_INCLUDE */



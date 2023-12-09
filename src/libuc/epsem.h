/* psem INCLUDE */
/* lang=C20 */


/* revision history:

	= 1999-07-28, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/*******************************************************************************

	This module implements an emulated POSIX semaphore.  I have
	to write a look-alike implementation of POSIX semaphores
	myself because the Apple Darwin implemention does not allow
	for the un-named semaphore use case!  Yes, once and (and again,
	and again) the Apple Darwin UNIX-like implementation sucks
	cock meat. F*ck Apple Darwin.

*******************************************************************************/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	PSEM_INCLUDE
#define	PSEM_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<clanguage.h>
#include	<csem.h>		/* <- couting semaphore (DAM) */

#define	PSEM		CSEM		/* <- the "money" shot */

#ifndef	TYPEDEF_PSEM
#define	TYPEDEF_PSEM
typedef PSEM		psem ;
#endif

#ifdef	__cplusplus
extern "C" {
#endif

extern int	psem_create(PSEM *,int,int) noex ;
extern int	psem_destroy(PSEM *) noex ;
extern int	psem_wait(PSEM *) noex ;
extern int	psem_waiter(PSEM *,int) noex ;
extern int	psem_trywait(PSEM *) noex ;
extern int	psem_post(PSEM *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* PSEM_INCLUDE */



/* rpsem HEADER ("real" POSIX� Semaphore) */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* POSIX� unnamed Semaphore (PSEM) */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-28, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright � 1999 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	psem

	Description:
	These are "unnamed" POSIX� semaphores.

*******************************************************************************/

#ifndef	PSEM_INCLUDE
#define	PSEM_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<semaphore.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	PSEM		sem_t


#ifndef	TYPEDEF_PSEM
#define	TYPEDEF_PSEM
typedef PSEM		psem ;
#endif

EXTERNC_begin

extern int	psem_create(psem *,int,int) noex ;
extern int	psem_destroy(psem *) noex ;
extern int	psem_wait(psem *) noex ;
extern int	psem_waiter(psem *,int) noex ;
extern int	psem_trywait(psem *) noex ;
extern int	psem_post(psem *) noex ;
extern int	psem_count(psem *) noex ;

EXTERNC_end


#endif /* PSEM_INCLUDE */



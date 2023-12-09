/* lockfile INCLUDE */
/* lang=C20 */

/* sister subroutine to |uc_lockf(3uc)| */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	LOCKFILE_INCLUDE
#define	LOCKFILE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int	lockfile(int,int,off_t,off_t,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* LOCKFILE_INCLUDE */



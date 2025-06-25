/* stackaddr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* stack-address management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A�D� Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	STACKADDR_INCLUDE
#define	STACKADDR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	STACKADDR	struct stackaddr_head


struct stackaddr_head {
	char		*dbuf ;
	cchar		*lhp ;
	int		lhl ;
	int		dlen ;
	int		i ;
	int		ri ;
} ;

typedef	STACKADDR	stackaddr ;

EXTERNC_begin

extern int	stackaddr_start(stackaddr *,char *,int) noex ;
extern int	stackaddr_finish(stackaddr *) noex ;
extern int	stackaddr_add(stackaddr *,cchar *,int,cchar *,int) noex ;

EXTERNC_end


#endif /* STACKADDR_INCLUDE */



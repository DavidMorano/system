/* getlogx HEADER */
/* lang=C20 */

/* get the name of the controlling terminal for the current session */
/* version %I% last-modified %G% */


/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	GETLOGX_INCLUDE
#define	GETLOGX_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* |pid_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	getlogname(char *,int,pid_t) noex ;
extern int	getlogterm(char *,int,pid_t) noex ;
extern int	getloghost(char *,int,pid_t) noex ;

EXTERNC_end


#endif /* GETLOGX_INCLUDE */



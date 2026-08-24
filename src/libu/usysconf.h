/* usysconf HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* File-Once management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USYSCONF_INCLUDE
#define	USYSCONF_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<sysconfcmds.h>		/* LIBU */


EXTERNC_begin

extern int	u_sysconfval	(int,long *) noex ;
extern int	u_sysconfstr	(int,char *,int) noex ;
extern int	u_nprocessors	(int) noex ;

EXTERNC_end

#ifdef	__cplusplus
    struct unprocer {
	operator int () noex ;
	int operator () (int = 0) noex ;
    } ; /* end struct */
    extern unprocer 	unprocessors ;
#endif /* __cplusplus */


#endif /* USYSCONF_INCLUDE */



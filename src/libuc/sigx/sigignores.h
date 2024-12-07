/* sigignores HEADER */
/* encoding=ISO8859-1 */
/* lang=C99 */

/* ignores a group of signals */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sigignores

	Description:
	This subroutine sets the disposition of the specified set
	of signals (given as an array of integers containing the
	desired signal numbers) such that those signals will be
	ignored.

*******************************************************************************/

#ifndef	SIGIGNORES_INCLUDE
#define	SIGIGNORES_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>


EXTERNC_begin

extern int	sigignores(cint *) noex ;

EXTERNC_end


#endif /* SIGIGNORES_INCLUDE */



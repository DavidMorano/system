/* getnodename HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get the node-name of the current system */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-03, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETNODENAME_INCLUDE
#define	GETNODENAME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	getnodename(char *,int) noex ;

EXTERNC_end


#endif /* GETNODENAME_INCLUDE */



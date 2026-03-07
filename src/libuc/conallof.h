/* conallof HEADER (contains-all-of) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* does the given string have all of the given characters? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This module was originally written for hardware CAD support.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CONALLOF_INCLUDE
#define	CONALLOF_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	conallof(cchar *,int,cchar *) noex ;

EXTERNC_end


#endif /* CONALLOF_INCLUDE */



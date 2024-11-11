/* getrunlevel HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* UNIX® retrieve the run-level of the system */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This file contains some support for the UNIX® Address-Familt
	(AF) related operations.

*******************************************************************************/

#ifndef	GETRUNLEVEL_INCLUDE
#define	GETRUNLEVEL_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	getrunlevel(cchar *) noex ;

EXTERNC_end


#endif /* GETRUNLEVEL_INCLUDE */



/* prmktmpdir HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* make a temporaty directory for a given program-root */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A�D� Morano.  All rights reserved. */

#ifndef	PRMKTMPDIR_INCLUDE
#define	PRMKTMPDIR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	prmktmpdir(cchar *,char *,cc *,cc *,mode_t) noex ;

EXTERNC_end


#endif /* PRMKTMPDIR_INCLUDE */



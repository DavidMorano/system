/* hasmodname HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* has a counted c-string contain a module-name ? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HASMODNAME_INCLUDE
#define	HASMODNAME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

EXTERNC_begin

extern bool	hasmodname(cchar *,int) noex ;

EXTERNC_end


#endif /* HASMODNAME_INCLUDE */



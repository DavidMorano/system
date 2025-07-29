/* mkunique HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* create a c-string w/ all unique characters */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MKUNIQUE_INCLUDE
#define	MKUNIQUE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	mkunique(char *,int,cchar *,int) noex ;

EXTERNC_end


#endif /* MKUNIQUE_INCLUDE */



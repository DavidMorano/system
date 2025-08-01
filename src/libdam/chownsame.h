/* chownsame HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* load a CHOWNSAME module and call it */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CHOWNSAME_INCLUDE
#define	CHOWNSAME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern int	chownsame(cchar *,cchar *) noex ;
extern int	chownref(cchar *,cchar *) noex ;

EXTERNC_end


#endif /* CHOWNSAME_INCLUDE */



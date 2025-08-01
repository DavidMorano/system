/* cpuspeed HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* load a CPUSPEED module and call it */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CPUSPEED_INCLUDE
#define	CPUSPEED_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern int	cpuspeed(cchar *,cchar *,int) noex ;

EXTERNC_end


#endif /* CPUSPEED_INCLUDE */



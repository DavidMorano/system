/* strvalcmp HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find a substring within a larger string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRVALCMP_INCLUDE
#define	STRVALCMP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern int	strvalcmp(cchar *,cchar *) noex ;

extern int	strxvalcmp(cchar *,cchar *) noex ;

EXTERNC_end


#endif /* STRVALCMP_INCLUDE */



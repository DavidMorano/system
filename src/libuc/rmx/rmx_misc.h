/* rmx_misc HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* remove characters from the end of the given counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	RMXMISC_INCLUDE
#define	RMXMISC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern int	rmext(cchar *,int) noex ;
extern int	rmcomment(cchar *,int) noex ;

EXTERNC_end


#endif /* RMXMISC_INCLUDE */



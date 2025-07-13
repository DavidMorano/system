/* siext HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* determine if file-name (just a string) has an approved file-extension */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SIEXT_INCLUDE
#define	SIEXT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	siext(cchar *,int) noex ;

EXTERNC_end


#endif /* SIEXT_INCLUDE */



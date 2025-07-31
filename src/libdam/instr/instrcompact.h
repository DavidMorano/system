/* instrcompact HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* remove the whitespace out of a c-string in-place */
/* version %I% last-modified %G% */


/* revision history:

	= 1992-05-14, David A­D­ Morano
	This was originally written for a UNIX-based hardware 
	support tool.

*/

/* Copyright © 1992 David A­D­ Morano.  All rights reserved. */

#ifndef	INSTRCOMPACT_INCLUDE
#define	INSTRCOMPACT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	instrcompact(char *,int) noex ;

EXTERNC_end


#endif /* INSTRCOMPACT_INCLUDE */



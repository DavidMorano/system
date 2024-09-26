/* strx HEADER */
/* lang=C20 */

/* c-string comparisons */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRX_INCLUDE
#define	STRX_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

#include	<strxcmp.h>


EXTERNC_begin

extern int	strwildsub(cchar *,cchar *) noex ;
extern int	strwildsub(cchar *,cchar *) noex ;

extern char	*strrpbrk(cchar *,cchar *) noex ;
extern char	*strwhite(cchar *) noex ;

EXTERNC_end


#endif /* STRX_INCLUDE */



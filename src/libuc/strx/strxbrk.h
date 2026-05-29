/* strxbrk HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* c-string comparisons */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-06-08, David A­D­ Morano
	This code was originally written for some reason.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRXBRK_INCLUDE
#define	STRXBRK_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern char	*strobrk(cchar *,cchar *) noex ;
extern char	*strrbrk(cchar *,cchar *) noex ;

EXTERNC_end


#endif /* STRXBRK_INCLUDE */



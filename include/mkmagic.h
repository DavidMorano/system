/* mkmagic HEADER */
/* encoding=ISO8859-1 */
/* lang=C99 */

/* create a c-string that constitutes a "magic" string for file-ID purposes */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-02-01, David A­D­ Morano
        This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

#ifndef	MKMAGIC_INCLUDE
#define	MKMAGIC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	mkmagic(char *,int,cchar *) noex ;

EXTERNC_end


#endif /* MKMAGIC_INCLUDE */



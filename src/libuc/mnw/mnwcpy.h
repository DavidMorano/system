/* mnwcpy HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* counted memory copy stuff (string or buffer) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MNWPCYINCLUDE
#define	MNWPCYINCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */


EXTERNC_begin

extern int	mnwcpy(char *,int,cchar *,int) noex ;

EXTERNC_end


#endif /* MNWPCYINCLUDE */



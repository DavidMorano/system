/* headketmat HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* match on mail-message header keys */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HEADKEYMAT_INCLUDE
#define	HEADKEYMAT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	headkeymat(cchar *,cchar *,int) noex ;

EXTERNC_end


#endif /* HEADKEYMAT_INCLUDE */



/* recip_copyparts HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* copies parts of a file to another file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	RECIPCOPYPARTS_INCLUDE
#define	RECIPCOPYPARTS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<recip.h>


EXTERNC_begin

extern int	recip_copyparts(recip *,int,int) noex ;

EXTERNC_end


#endif /* RECIPCOPYPARTS_INCLUDE */



/* sileader HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find index to first gicen character after skipping white-space */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SILEADER_INCLUDE
#define	SILEADER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	sileader(cchar *,int) noex ;

EXTERNC_end


#endif /* SILEADER_INCLUDE */



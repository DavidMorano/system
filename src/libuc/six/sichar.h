/* sinon HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find a character or white-space */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SICHAR_INCLUDE
#define	SICHAR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	sialpha(cchar *,int) noex ;
extern int	sialnum(cchar *,int) noex ;
extern int	sidigit(cchar *,int) noex ;
extern int	sixdigit(cchar *,int) noex ;

EXTERNC_end


#endif /* SICHAR_INCLUDE */



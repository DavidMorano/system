/* strnvalcmp HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* c-string key comparisons */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRNVALCMP_INCLUDE
#define	STRNVALCMP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin
extern int	strnvalcmp(cchar *,cchar *,int = -1) noex ;
EXTERNC_end


#endif /* STRNVALCMP_INCLUDE */



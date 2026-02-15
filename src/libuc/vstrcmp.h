/* vstrcmp HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* convert a character string to a given case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	VSTRCMP_INCLUDE
#define	VSTRCMP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

#include	<vstrkeycmp.h>


EXTERNC_begin

extern int	vstrbasecmp(cchar **,cchar **) noex ;
extern int	vstrcasecmp(cchar **,cchar **) noex ;
extern int	vstrfoldcmp(cchar **,cchar **) noex ;

static inline int vstrcmp(cchar **s1pp,cchar **s2pp) noex {
	return vstrbasecmp(s1pp,s2pp) ;
}

EXTERNC_end


#endif /* VSTRCMP_INCLUDE */



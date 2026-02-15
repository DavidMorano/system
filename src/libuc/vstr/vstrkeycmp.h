/* vstrkeycmp HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* v-string key-comparison functions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	VSTRKEYCMP_INCLUDE
#define	VSTRKEYCMP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern int	vstrkeybasecmp(cchar **,cchar **) noex ;
extern int	vstrkeycasecmp(cchar **,cchar **) noex ;
extern int	vstrkeyfoldcmp(cchar **,cchar **) noex ;

static inline int vstrkeycmp(cchar **s1pp,cchar **s2pp) noex {
	return vstrkeybasecmp(s1pp,s2pp) ;
}

EXTERNC_end


#endif /* VSTRKEYCMP_INCLUDE */



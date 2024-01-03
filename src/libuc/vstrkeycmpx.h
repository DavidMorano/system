/* vstrkeycmpx HEADER */
/* lang=C20 */

/* v-string key-comparison functions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A�D� Morano.  All rights reserved. */

#ifndef	VSTRKEYCMPX_INCLUDE
#define	VSTRKEYCMPX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int vstrkeycmpo(cchar **,cchar **) noex ;
extern int vstrkeycmpr(cchar **,cchar **) noex ;
extern int vstrkeydictcmp(cchar **,cchar **) noex ;

static inline int vstrkeycmp(cchar **s1pp,cchar **s2pp) noex {
	return vstrkeycmpo(s1pp,s2pp) ;
}

#ifdef	__cplusplus
}
#endif


#endif /* VSTRKEYCMPX_INCLUDE */



/* ucgetua HEADER */
/* lang=C20 */

/* UNIX® C-language system database access (UCGET) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCGETUA_INCLUDE
#define	UCGETUA_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<ucentua.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int	uc_userattrbegin() noex ;
extern int	uc_getuaent(ucentua *,char *,int) noex ;
extern int	uc_getuanam(ucentua *,char *,int,cchar *) noex ;
extern int	uc_getuauid(ucentua *,char *,int,uid_t) noex ;
extern int	uc_userattrend() noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UCGETUA_INCLUDE */



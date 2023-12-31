/* ucgetho HEADER */
/* lang=C20 */

/* UNIX� C-language system database access (UCGET) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This program was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	UCGETHO_INCLUDE
#define	UCGETHO_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<ucentho.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int	uc_gethobegin(int) noex ;
extern int	uc_gethoent(ucentho *,char *,int) noex ;
extern int	uc_gethonam(ucentho *,char *,int,cchar *) noex ;
extern int	uc_gethoadd(ucentho *,char *,int,cvoid *,int,int) noex ;
extern int	uc_gethoend() noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UCGETHO_INCLUDE */



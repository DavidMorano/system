/* ucgetsp HEADER */
/* lang=C20 */

/* UNIX� C-language system database access (UCGET) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A�D� Morano
	This module was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	UCGETSP_INCLUDE
#define	UCGETSP_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<ucentsp.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int	uc_getspbegin() noex ;
extern int	uc_getspent(ucentsp *,char *,int) noex ;
extern int	uc_getspnam(ucentsp *,char *,int,cchar *) noex ;
extern int	uc_getspend() noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UCGETSP_INCLUDE */



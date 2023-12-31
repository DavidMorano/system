/* ucgetpw HEADER */
/* lang=C20 */

/* UNIX� C-language system database access (UCGET) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A�D� Morano
	This module was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	UCGETPW_INCLUDE
#define	UCGETPW_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<ucentpw.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int	uc_getpwbegin() noex ;
extern int	uc_getpwent(ucentpw *,char *,int) noex ;
extern int	uc_getpwnam(ucentpw *,char *,int,cchar *) noex ;
extern int	uc_getpwuid(ucentpw *,char *,int,uid_t) noex ;
extern int	uc_getpwend() noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UCGETPW_INCLUDE */



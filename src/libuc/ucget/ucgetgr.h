/* ucgetgr INCLUDE */
/* lang=C20 */

/* UNIX® C-language system database access (UCGET) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCGETGR_INCLUDE
#define	UCGETGR_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<ucentgr.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int	uc_getgrbegin() noex ;
extern int	uc_getgrent(ucentgr *,char *,int) noex ;
extern int	uc_getgrnam(ucentgr *,char *,int,cchar *) noex ;
extern int	uc_getgruid(ucentgr *,char *,int,uid_t) noex ;
extern int	uc_getgrend() noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UCGETGR_INCLUDE */



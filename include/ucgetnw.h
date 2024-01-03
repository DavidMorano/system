/* ucgetnw HEADER */
/* lang=C20 */

/* UNIX® C-language system database access (UCGET) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCGETNW_INCLUDE
#define	UCGETNW_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<ucentnw.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int	uc_getnwbegin(int) noex ;
extern int	uc_getnwent(ucentnw *,char *,int) noex ;
extern int	uc_getnwnam(ucentnw *,char *,int,cchar *) noex ;
extern int	uc_getnwnum(ucentnw *,char *,int,uint32_t,int) noex ;
extern int	uc_getnwend() noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UCGETNW_INCLUDE */



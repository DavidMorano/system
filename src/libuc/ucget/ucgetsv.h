/* ucgetsv HEADER */
/* lang=C20 */

/* UNIX® C-language system database access (UCGET) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCGETSV_INCLUDE
#define	UCGETSV_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<ucentsv.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int	uc_getsvbegin(int) noex ;
extern int	uc_getsvent(ucentsv *,char *,int) noex ;
extern int	uc_getsvnam(ucentsv *,char *,int,cchar *,cchar *) noex ;
extern int	uc_getsvnum(ucentsv *,char *,int,int,cchar *) noex ;
extern int	uc_getsvend() noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UCGETSV_INCLUDE */



/* ucgetnw HEADER */
/* charset=ISO8859-1 */
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


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucentnw.h>		/* LIBUC */


EXTERNC_begin

extern int	uc_getnwbegin	(int) noex ;
extern int	uc_getnwent	(ucentnw *,char *,int) noex ;
extern int	uc_getnwnam	(ucentnw *,char *,int,cchar *) noex ;
extern int	uc_getnwnum	(ucentnw *,char *,int,int,uint32_t) noex ;
extern int	uc_getnwend	() noex ;

EXTERNC_end


#endif /* UCGETNW_INCLUDE */



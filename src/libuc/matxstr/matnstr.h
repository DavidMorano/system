/* matnstr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Match-Option-String */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MATNSTR_INCLUDE
#define	MATNSTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern int	matnbasestr(con mainv,cchar *,int) noex ;
extern int	matncasestr(con mainv,cchar *,int) noex ;
extern int	matnfoldstr(con mainv,cchar *,int) noex ;

local inline int matnstr(con mainv a,cchar *sp,int sl) noex {
    	return matnbasestr(a,sp,sl) ;
}

EXTERNC_end


#endif /* MATNSTR_INCLUDE */



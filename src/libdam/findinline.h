/* findinline HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find a TeX-type in-line text escape */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This module was changed to serve in the REFERM program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	FINDINLINE_INCLUDE
#define	FINDINLINE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	FINDINLINE	struct findinline_head


struct findinline_head {
	cchar		*sp ;		/* "start" pointer */
	cchar		*kp, *vp ;
	int		kl, vl ;
} ; /* end struct */

typedef FINDINLINE	findinline ;

EXTERNC_begin

extern int	findinline_esc(findinline *,cchar *,int) noex ;

EXTERNC_end


#endif /* FINDINLINE_INCLUDE */



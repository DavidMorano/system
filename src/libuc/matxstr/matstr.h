/* matstr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Match a String */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MATSTR_INCLUDE
#define	MATSTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */

#include	<matxstr.h>


EXTERNC_begin

extern int matbasestr(con mainv,cchar *,int) noex ;
extern int matcasestr(con mainv,cchar *,int) noex ;
extern int matfoldstr(con mainv,cchar *,int) noex ;

EXTERNC_end

#ifdef	__cplusplus
inline int matstr(con mainv bs,cchar *sp,int sl = -1) noex {
	return matbasestr(bs,sp,sl) ;
}
#else
local inline int matstr(con mainv bs,cchar *sp,int sl) noex {
	return matbasestr(bs,sp,sl) ;
}
#endif /* __cplusplus */


#endif /* MATSTR_INCLUDE */



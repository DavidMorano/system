/* matsubstr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Match-Option-String */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MATSUBSTR_INCLUDE
#define	MATSUBSTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern int matbasesubstr(con mainv,int,cchar *,int) noex ;
extern int matcasesubstr(con mainv,int,cchar *,int) noex ;
extern int matfoldsubstr(con mainv,int,cchar *,int) noex ;

EXTERNC_end

#ifdef	__cplusplus
inline int matsubstr(con mainv bs,int n,cchar *sp,int sl = -1) noex {
	return matbasesubstr(bs,n,sp,sl) ;
}
#else
local inline int matsubstr(con mainv bs,int n,cchar *sp,int sl) noex {
	return matbasesubstr(bs,n,sp,sl) ;
}
#endif /* __cplusplus */


#endif /* MATSUBSTR_INCLUDE */



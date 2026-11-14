/* comstrx HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* match on the leading part of a string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	COMSTRX_INCLUDE
#define	COMSTRX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_beg
extern int comstrbc(cchar *,cchar *,int) noex ;
extern int comstric(cchar *,cchar *,int) noex ;
extern int comstrfc(cchar *,cchar *,int) noex ;
EXTERNC_end

EXTERNC_beg
local inline int comstr(cchar *bs,cchar *sp,int sl) noex {
	return comstrbc(bs,sp,sl) ;
} /* end */
EXTERNC_end

#ifdef	__cplusplus
inline int comstr(cchar *bs,cchar *sp) noex {
	return comstrbc(bs,sp,-1) ;
} /* end */
#endif /* __cplusplus */


#endif /* COMSTRX_INCLUDE */



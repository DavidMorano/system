/* comstrk HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* match on the leading part of a string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	COMSTRK_INCLUDE
#define	COMSTRK_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_beg
    extern int comstrkbc(cchar *bs,cchar *sp,int sl) noex ;
    extern int comstrkic(cchar *bs,cchar *sp,int sl) noex ;
    extern int comstrkfc(cchar *bs,cchar *sp,int sl) noex ;
EXTERNC_end

EXTERNC_beg
    inline int comstrk(cchar *bs,cchar *sp,int sl) noex {
    	return comstrkbc(bs,sp,sl) ;
    } /* end */
EXTERNC_end

#ifdef	__cplusplus
    inline int comstrkbc(cchar *bs,cchar *sp) noex {
    	return comstrkbc(bs,sp,-1) ;
    } /* end */
    inline int comstrkic(cchar *bs,cchar *sp) noex {
    	return comstrkic(bs,sp,-1) ;
    } /* end */
    inline int comstrkfc(cchar *bs,cchar *sp) noex {
    	return comstrkfc(bs,sp,-1) ;
    } /* end */
#endif /* __cplusplus */

#ifdef	__cplusplus
    inline int comstrk(cchar *bs,cchar *sp) noex {
    	return comstrkbc(bs,sp,-1) ;
    } /* end */
#endif /* __cplusplus */


#endif /* COMSTRK_INCLUDE */



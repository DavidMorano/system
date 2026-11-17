/* sisubx HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find a substring in a c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SISUBX_INCLUDE
#define	SISUBX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_beg
    extern int sisubbco(cchar *sp,int sl,cchar *ss) noex ;
    extern int sisubbcr(cchar *sp,int sl,cchar *ss) noex ;
    extern int sisubico(cchar *sp,int sl,cchar *ss) noex ;
    extern int sisubicr(cchar *sp,int sl,cchar *ss) noex ;
    extern int sisubfco(cchar *sp,int sl,cchar *ss) noex ;
    extern int sisubfcr(cchar *sp,int sl,cchar *ss) noex ;
EXTERNC_end

EXTERNC_beg
    inline int sisubo(cchar *sp,int sl,cchar *ss) noex {
	return sisubbco(sp,sl,ss) ;
    } /* end */
    inline int sisubr(cchar *sp,int sl,cchar *ss) noex {
	return sisubbcr(sp,sl,ss) ;
    } /* end */
EXTERNC_end

EXTERNC_beg
    inline int sisub(cchar *sp,int sl,cchar *ss) noex {
	return sisubo(sp,sl,ss) ;
    } /* end */
EXTERNC_end


#endif /* SISUBX_INCLUDE */



/* cmpstrkeyn HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* c-string comparisons w/ maximum length and case variations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CMPSTRKEYN_INCLUDE
#define	CMPSTRKEYN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_beg
extern int	cmpstrkeynbc		(cchar *,cchar *,int) noex ;
extern int	cmpstrkeynic		(cchar *,cchar *,int) noex ;
extern int	cmpstrkeynfc		(cchar *,cchar *,int) noex ;
EXTERNC_end

EXTERNC_beg
local inline int cmpstrkeyn		(cchar *a,cchar *sp,int sl) noex {
    	return cmpstrkeynbc(a,sp,sl) ;
} /* end */
EXTERNC_end

EXTERNC_beg
extern int	cmpstrkeynbco		(cchar *,cchar *,int) noex ;
extern int	cmpstrkeynbcr		(cchar *,cchar *,int) noex ;
EXTERNC_end

EXTERNC_beg
extern int	cmpstrkeynico		(cchar *,cchar *,int) noex ;
extern int	cmpstrkeynicr		(cchar *,cchar *,int) noex ;
EXTERNC_end

EXTERNC_beg
extern int	cmpstrkeynfco		(cchar *,cchar *,int) noex ;
extern int	cmpstrkeynfcr		(cchar *,cchar *,int) noex ;
EXTERNC_end

EXTERNC_beg
extern int	cmpstrkeynøbco	(cchar *,cchar *,int) noex ;
extern int	cmpstrkeynøbcr	(cchar *,cchar *,int) noex ;
EXTERNC_end

EXTERNC_beg
extern int	cmpstrkeynøico	(cchar *,cchar *,int) noex ;
extern int	cmpstrkeynøicr	(cchar *,cchar *,int) noex ;
EXTERNC_end

EXTERNC_beg
extern int	cmpstrkeynøfco	(cchar *,cchar *,int) noex ;
extern int	cmpstrkeynøfcr	(cchar *,cchar *,int) noex ;
EXTERNC_end

EXTERNC_beg
local inline int cmpstrkeynøbc	(cchar *s1,cchar *s2,int n) noex {
    	return cmpstrkeynøbco(s1,s2,n) ;
} /* end */
local inline int cmpstrkeynøic	(cchar *s1,cchar *s2,int n) noex {
    	return cmpstrkeynøico(s1,s2,n) ;
} /* end */
local inline int cmpstrkeynøfc	(cchar *s1,cchar *s2,int n) noex {
    	return cmpstrkeynøfco(s1,s2,n) ;
} /* end */
EXTERNC_end

#ifdef	__cplusplus
inline int	cmpstrkeyn	(cchar *s1,cchar *s2) noex {
    	return cmpstrkeyn(s1,s2,-1) ;
} /* end */
#endif /* __cplusplus */


#endif /* CMPSTRKEYN_INCLUDE */



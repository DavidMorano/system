/* cmpstrnx HEADER (Compre-String-Maximum) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* c-string comparisons w/ maximum length and case variations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CMPSTRNX_INCLUDE
#define	CMPSTRNX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_beg
extern int	cmpstrnbc	(cchar *,cchar *,int) noex ;	/* base */
extern int	cmpstrnic	(cchar *,cchar *,int) noex ;	/* insenstive */
extern int	cmpstrnfc	(cchar *,cchar *,int) noex ;	/* fold */
EXTERNC_end

EXTERNC_beg
extern int	cmpstrnbco	(cchar *,cchar *,int) noex ;
extern int	cmpstrnbcr	(cchar *,cchar *,int) noex ;
EXTERNC_end

EXTERNC_beg
extern int	cmpstrnico	(cchar *,cchar *,int) noex ;
extern int	cmpstrnicr	(cchar *,cchar *,int) noex ;
EXTERNC_end

EXTERNC_beg
extern int	cmpstrnfco	(cchar *,cchar *,int) noex ;
extern int	cmpstrnfcr	(cchar *,cchar *,int) noex ;
EXTERNC_end

EXTERNC_beg
extern int	cmpstrnøbco	(cchar *,cchar *,int) noex ;
extern int	cmpstrnøbcr	(cchar *,cchar *,int) noex ;
EXTERNC_end

EXTERNC_beg
extern int	cmpstrnøico	(cchar *,cchar *,int) noex ;
extern int	cmpstrnøicr	(cchar *,cchar *,int) noex ;
EXTERNC_end

EXTERNC_beg
extern int	cmpstrnøfco	(cchar *,cchar *,int) noex ;
extern int	cmpstrnøfcr	(cchar *,cchar *,int) noex ;
EXTERNC_end

EXTERNC_beg
local inline int cmpstrnøbc	(cchar *s1,cchar *s2,int n) noex {
    	return cmpstrnøbco(s1,s2,n) ;
} /* end */
local inline int cmpstrnøic	(cchar *s1,cchar *s2,int n) noex {
    	return cmpstrnøico(s1,s2,n) ;
} /* end */
local inline int cmpstrnøfc	(cchar *s1,cchar *s2,int n) noex {
    	return cmpstrnøfco(s1,s2,n) ;
} /* end */
EXTERNC_end


#endif /* CMPSTRNX_INCLUDE */



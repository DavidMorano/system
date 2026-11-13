/* cmpstrx HEADER (Compre-String) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* c-string comparisons w/ case variations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-23, David A-D- Morano
	This was written for Rightcore Network Services (RNS).
 
*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CMPSTRX_INCLUDE
#define	CMPSTRX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_beg
extern int	cmpstrbc(cchar *,cchar *) noex ;	/* case-base */
extern int	cmpstric(cchar *,cchar *) noex ;	/* case-insensitive */
extern int	cmpstrfc(cchar *,cchar *) noex ;	/* case-fold */
EXTERNC_end

EXTERNC_beg
extern int	cmpstrbco(cchar *,cchar *) noex ;
extern int	cmpstrbcr(cchar *,cchar *) noex ;
EXTERNC_end

EXTERNC_beg
extern int	cmpstrico(cchar *,cchar *) noex ;
extern int	cmpstricr(cchar *,cchar *) noex ;
EXTERNC_end

EXTERNC_beg
extern int	cmpstrfco(cchar *,cchar *) noex ;
extern int	cmpstrfcr(cchar *,cchar *) noex ;
EXTERNC_end

EXTERNC_beg
extern int	cmpstrøbco(cchar *,cchar *) noex ;
extern int	cmpstrøbcr(cchar *,cchar *) noex ;
EXTERNC_end

EXTERNC_beg
extern int	cmpstrøico(cchar *,cchar *) noex ;
extern int	cmpstrøicr(cchar *,cchar *) noex ;
EXTERNC_end

EXTERNC_beg
extern int	cmpstrøfco(cchar *,cchar *) noex ;
extern int	cmpstrøfcr(cchar *,cchar *) noex ;
EXTERNC_end

EXTERNC_beg
local inline int cmpstrøbc(cchar *s1,cchar *s2) noex {
    	return cmpstrøbco(s1,s2) ;
} /* end */
local inline int cmpstrøic(cchar *s1,cchar *s2) noex {
    	return cmpstrøico(s1,s2) ;
} /* end */
local inline int cmpstrøfc(cchar *s1,cchar *s2) noex {
    	return cmpstrøfco(s1,s2) ;
} /* end */
EXTERNC_end


#endif /* CMPSTRX_INCLUDE */



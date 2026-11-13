/* cmpstrk HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* make a character out of an integer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This was written for Rightcore Network Services (RNS).
 
*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CMPSTRK_INCLUDE
#define	CMPSTRK_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin
extern int cmpstrkbco(cchar *,cchar *) noex ;
extern int cmpstrkbcr(cchar *,cchar *) noex ;
EXTERNC_end

EXTERNC_begin
extern int cmpstrkico(cchar *,cchar *) noex ;
extern int cmpstrkicr(cchar *,cchar *) noex ;
EXTERNC_end

EXTERNC_begin
extern int cmpstrkfco(cchar *,cchar *) noex ;
extern int cmpstrkfcr(cchar *,cchar *) noex ;
EXTERNC_end

EXTERNC_begin
extern int cmpstrkbc(cchar *,cchar *) noex ;
extern int cmpstrkic(cchar *,cchar *) noex ;
extern int cmpstrkfc(cchar *,cchar *) noex ;
EXTERNC_end

EXTERNC_begin
local inline int cmpstrk(cchar *s1,cchar *s2) noex {
    	return cmpstrkbc(s1,s2) ;
} /* end */
EXTERNC_end

EXTERNC_begin
extern int cmpstrkøbco(cchar *,cchar *) noex ;
extern int cmpstrkøbcr(cchar *,cchar *) noex ;
EXTERNC_end

EXTERNC_begin
extern int cmpstrkøico(cchar *,cchar *) noex ;
extern int cmpstrkøicr(cchar *,cchar *) noex ;
EXTERNC_end

EXTERNC_begin
extern int cmpstrkøfco(cchar *,cchar *) noex ;
extern int cmpstrkøfcr(cchar *,cchar *) noex ;
EXTERNC_end

EXTERNC_begin
local inline int cmpstrkøbc(cchar *s1,cchar *s2) noex {
    	return cmpstrkøbco(s1,s2) ;
} /* end */
local inline int cmpstrkøic(cchar *s1,cchar *s2) noex {
    	return cmpstrkøico(s1,s2) ;
} /* end */
local inline int cmpstrkøfc(cchar *s1,cchar *s2) noex {
    	return cmpstrkøfco(s1,s2) ;
} /* end */
EXTERNC_end


#endif /* CMPSTRK_INCLUDE */



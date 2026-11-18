/* cmpstrkey HEADER */
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
#include	<cmpstrkeyn.h>		/* LIBU */


EXTERNC_begin
extern int cmpstrkeybco(cchar *,cchar *) noex ;
extern int cmpstrkeybcr(cchar *,cchar *) noex ;
EXTERNC_end

EXTERNC_begin
extern int cmpstrkeyico(cchar *,cchar *) noex ;
extern int cmpstrkeyicr(cchar *,cchar *) noex ;
EXTERNC_end

EXTERNC_begin
extern int cmpstrkeyfco(cchar *,cchar *) noex ;
extern int cmpstrkeyfcr(cchar *,cchar *) noex ;
EXTERNC_end

EXTERNC_begin
extern int cmpstrkeybc(cchar *,cchar *) noex ;
extern int cmpstrkeyic(cchar *,cchar *) noex ;
extern int cmpstrkeyfc(cchar *,cchar *) noex ;
EXTERNC_end

EXTERNC_begin
local inline int cmpstrkey(cchar *s1,cchar *s2) noex {
    	return cmpstrkeybc(s1,s2) ;
} /* end */
EXTERNC_end

EXTERNC_begin
extern int cmpstrkeyøbco(cchar *,cchar *) noex ;
extern int cmpstrkeyøbcr(cchar *,cchar *) noex ;
EXTERNC_end

EXTERNC_begin
extern int cmpstrkeyøico(cchar *,cchar *) noex ;
extern int cmpstrkeyøicr(cchar *,cchar *) noex ;
EXTERNC_end

EXTERNC_begin
extern int cmpstrkeyøfco(cchar *,cchar *) noex ;
extern int cmpstrkeyøfcr(cchar *,cchar *) noex ;
EXTERNC_end

EXTERNC_begin
local inline int cmpstrkeyøbc(cchar *s1,cchar *s2) noex {
    	return cmpstrkeyøbco(s1,s2) ;
} /* end */
local inline int cmpstrkeyøic(cchar *s1,cchar *s2) noex {
    	return cmpstrkeyøico(s1,s2) ;
} /* end */
local inline int cmpstrkeyøfc(cchar *s1,cchar *s2) noex {
    	return cmpstrkeyøfco(s1,s2) ;
} /* end */
EXTERNC_end


#endif /* CMPSTRK_INCLUDE */



/* cmpvstr HEADER (Compare-V-String) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* v-string key comparison (w/ various cases) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CMPVSTR_INCLUDE
#define	CMPVSTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<cmporders.h>		/* LIBU */


EXTERNC_begin
extern int	cmpvstrbc(cchar **,cchar **) noex ;
extern int	cmpvstric(cchar **,cchar **) noex ;
extern int	cmpvstrfc(cchar **,cchar **) noex ;
EXTERNC_end

EXTERNC_begin
extern int	cmpvstrbcx(cchar **,cchar **,cmporders) noex ;
extern int	cmpvstricx(cchar **,cchar **,cmporders) noex ;
extern int	cmpvstrfcx(cchar **,cchar **,cmporders) noex ;
EXTERNC_end

EXTERNC_begin
local inline int cmpvstrbco(cchar **s1pp,cchar **s2pp) noex {
	return cmpvstrbcx(s1pp,s2pp,cmporder_obverse) ;
} /* end */
local inline int cmpvstrbcr(cchar **s1pp,cchar **s2pp) noex {
	return cmpvstrbcx(s1pp,s2pp,cmporder_reverse) ;
} /* end */
EXTERNC_end

EXTERNC_begin
local inline int cmpvstrico(cchar **s1pp,cchar **s2pp) noex {
	return cmpvstricx(s1pp,s2pp,cmporder_obverse) ;
} /* end */
local inline int cmpvstricr(cchar **s1pp,cchar **s2pp) noex {
	return cmpvstricx(s1pp,s2pp,cmporder_reverse) ;
} /* end */
EXTERNC_end

EXTERNC_begin
local inline int cmpvstrfco(cchar **s1pp,cchar **s2pp) noex {
	return cmpvstrfcx(s1pp,s2pp,cmporder_obverse) ;
} /* end */
local inline int cmpvstrfcr(cchar **s1pp,cchar **s2pp) noex {
	return cmpvstrfcx(s1pp,s2pp,cmporder_reverse) ;
} /* end */
EXTERNC_end

EXTERNC_begin
local inline int cmpvstr(cchar **s1pp,cchar **s2pp) noex {
	return cmpvstrbc(s1pp,s2pp) ;
} /* end */
local inline int cmpvstro(cchar **s1pp,cchar **s2pp) noex {
	return cmpvstrbco(s1pp,s2pp) ;
} /* end */
local inline int cmpvstrr(cchar **s1pp,cchar **s2pp) noex {
	return cmpvstrbcr(s1pp,s2pp) ;
} /* end */
EXTERNC_end


#endif /* CMPVSTR_INCLUDE */



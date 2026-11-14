/* cmpvstrk HEADER (Compare-V-String-Key) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* v-string key-comparison functions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CMPVSTRK_INCLUDE
#define	CMPVSTRK_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<cmporders.h>		/* LIBU */


EXTERNC_beg
    extern int	cmpvstrkbc(cchar **,cchar **) noex ;
    extern int	cmpvstrkic(cchar **,cchar **) noex ;
    extern int	cmpvstrkfc(cchar **,cchar **) noex ;
EXTERNC_end

EXTERNC_beg
    extern int	cmpvstrkbcx(cchar **,cchar **,cmporders) noex ;
    extern int	cmpvstrkicx(cchar **,cchar **,cmporders) noex ;
    extern int	cmpvstrkfcx(cchar **,cchar **,cmporders) noex ;
EXTERNC_end

EXTERNC_beg
    inline int cmpvstrkbco(cchar **s1pp,cchar **s2pp) noex {
	return cmpvstrkbcx(s1pp,s2pp,cmporder_obverse) ;
    } /* end subroutine */
    inline int cmpvstrkbcr(cchar **s1pp,cchar **s2pp) noex {
	return cmpvstrkbcx(s1pp,s2pp,cmporder_reverse) ;
    } /* end subroutine */
EXTERNC_end

EXTERNC_beg
    inline int cmpvstrkico(cchar **s1pp,cchar **s2pp) noex {
	return cmpvstrkicx(s1pp,s2pp,cmporder_obverse) ;
    } /* end subroutine */
    inline int cmpvstrkicr(cchar **s1pp,cchar **s2pp) noex {
	return cmpvstrkicx(s1pp,s2pp,cmporder_reverse) ;
    } /* end subroutine */
EXTERNC_end

EXTERNC_beg
    inline int cmpvstrkfco(cchar **s1pp,cchar **s2pp) noex {
	return cmpvstrkfcx(s1pp,s2pp,cmporder_obverse) ;
    } /* end subroutine */
    inline int cmpvstrkfcr(cchar **s1pp,cchar **s2pp) noex {
	return cmpvstrkfcx(s1pp,s2pp,cmporder_reverse) ;
    } /* end subroutine */
EXTERNC_end

EXTERNC_beg
    inline int cmpvstrk(cchar **s1pp,cchar **s2pp) noex {
	return cmpvstrkbc(s1pp,s2pp) ;
    } /* end subroutine */
    inline int cmpvstrko(cchar **s1pp,cchar **s2pp) noex {
	return cmpvstrkbcx(s1pp,s2pp,cmporder_obverse) ;
    } /* end subroutine */
    inline int cmpvstrkr(cchar **s1pp,cchar **s2pp) noex {
	return cmpvstrkbcx(s1pp,s2pp,cmporder_reverse) ;
    } /* end subroutine */
EXTERNC_end


#endif /* CMPVSTRK_INCLUDE */



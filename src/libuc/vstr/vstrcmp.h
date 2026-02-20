/* vstrcmp HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* convert a character string to a given case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	VSTRCMP_INCLUDE
#define	VSTRCMP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<vstrorder.h>

#include	<vstrkeycmp.h>


EXTERNC_begin

extern int	vstrbasecmp(cchar **,cchar **) noex ;
extern int	vstrcasecmp(cchar **,cchar **) noex ;
extern int	vstrfoldcmp(cchar **,cchar **) noex ;

extern int	vstrbasecmpx(cchar **,cchar **,vstrorders) noex ;
extern int	vstrcasecmpx(cchar **,cchar **,vstrorders) noex ;
extern int	vstrfoldcmpx(cchar **,cchar **,vstrorders) noex ;

local inline int vstrcmp(cchar **s1pp,cchar **s2pp) noex {
	return vstrbasecmp(s1pp,s2pp) ;
}
local inline int vstrcmpo(cchar **s1pp,cchar **s2pp) noex {
	return vstrbasecmpx(s1pp,s2pp,vstrorder_obverse) ;
}
local inline int vstrcmpr(cchar **s1pp,cchar **s2pp) noex {
	return vstrbasecmpx(s1pp,s2pp,vstrorder_reverse) ;
}

local inline int vstrbasecmpo(cchar **s1pp,cchar **s2pp) noex {
	return vstrbasecmpx(s1pp,s2pp,vstrorder_obverse) ;
}
local inline int vstrbasecmpr(cchar **s1pp,cchar **s2pp) noex {
	return vstrbasecmpx(s1pp,s2pp,vstrorder_reverse) ;
}

local inline int vstrcasecmpo(cchar **s1pp,cchar **s2pp) noex {
	return vstrcasecmpx(s1pp,s2pp,vstrorder_obverse) ;
}
local inline int vstrcasecmpr(cchar **s1pp,cchar **s2pp) noex {
	return vstrcasecmpx(s1pp,s2pp,vstrorder_reverse) ;
}

local inline int vstrfoldcmpo(cchar **s1pp,cchar **s2pp) noex {
	return vstrfoldcmpx(s1pp,s2pp,vstrorder_obverse) ;
}
local inline int vstrfoldcmpr(cchar **s1pp,cchar **s2pp) noex {
	return vstrfoldcmpx(s1pp,s2pp,vstrorder_reverse) ;
}

EXTERNC_end


#endif /* VSTRCMP_INCLUDE */



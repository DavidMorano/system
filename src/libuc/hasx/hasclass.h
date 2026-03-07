/* hasclass HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* does a counted c-string some characteristic? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HASCLASS_INCLUDE
#define	HASCLASS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern bool hasalpha(cchar *,int) noex ;
extern bool hasalnum(cchar *,int) noex ;
extern bool hasdigit(cchar *,int) noex ;
extern bool hasoctal(cchar *,int) noex ;
extern bool hasdigex(cchar *,int) noex ;
extern bool haswhite(cchar *,int) noex ;
extern bool hasblank(cchar *,int) noex ;
extern bool hasprint(cchar *,int) noex ;
extern bool hasterm(cchar *,int) noex ;

extern bool haslc(cchar *,int) noex ;
extern bool hasuc(cchar *,int) noex ;

local inline bool hasdig(cchar *sp,int sl) noex {
    	return hasdigit(sp,sl) ;
}
local inline bool hasoct(cchar *sp,int sl) noex {
    	return hasoctal(sp,sl) ;
}
local inline bool hasdec(cchar *sp,int sl) noex {
    	return hasdigit(sp,sl) ;
}
local inline bool hashex(cchar *sp,int sl) noex {
    	return hasdigex(sp,sl) ;
}
local inline bool haswht(cchar *sp,int sl) noex {
    	return haswhite(sp,sl) ;
}
local inline bool hasblk(cchar *sp,int sl) noex {
    	return hasblank(sp,sl) ;
}

EXTERNC_end


#endif /* HASCLASS_INCLUDE */



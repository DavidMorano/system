/* hasx HEADER */
/* lang=C20 */

/* has a counted c-string some characteristic? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HASX_INCLUDE
#define	HASX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<matxstr.h>
#include	<stdfnames.h>


EXTERNC_begin

extern bool haswhite(cchar *,int) noex ;
extern bool haslc(cchar *,int) noex ;
extern bool hasuc(cchar *,int) noex ;
extern bool hasdig(cchar *,int) noex ;
extern bool hasempty(cchar *,int) noex ;
extern bool hasalpha(cchar *,int) noex ;
extern bool hasalnum(cchar *,int) noex ;
extern bool hasnum(cchar *,int) noex ;

extern bool hasprintlatin(cchar *,int) noex ;
extern bool hasprintbad(cchar *,int) noex ;
extern bool hasdoublewhite(cchar *,int) noex ;
extern bool hasnonwhite(cchar *,int) noex ;
extern bool hasdots(cchar *,int) noex ;
extern bool hasvarpathprefix(cchar *,int) noex ;
extern bool hascdpath(cchar *,int) noex ;
extern bool hasmacro(cchar *,int) noex ;
extern bool hasnotdots(cchar *,int) noex ;
extern bool hasnotempty(cchar *,int) noex ;
extern bool haseoh(cchar *,int) noex ;
extern bool hasleadcolon(cchar *,int) noex ;
extern bool hasonlyminus(cchar *,int) noex ;
extern bool hasonlyplusminus(cchar *,int) noex ;

extern bool hasallalpha(cchar *,int) noex ;
extern bool hasallalnum(cchar *,int) noex ;
extern bool hasallwhite(cchar *,int) noex ;
extern bool hasalllc(cchar *,int) noex ;
extern bool hasalluc(cchar *,int) noex ;
extern bool hasalldig(cchar *,int) noex ;

extern bool hasallbase(cchar *,int,int) noex ;
extern bool hasallchr(cchar *,int,int) noex ;
extern bool hasvalidmagic(cchar *,int,cchar *) noex ;

static inline bool hasfnamespecial(cchar *fp,int fl) noex {
	return (matstr(stdfnames,fp,fl) >= 0) ;
}
static inline bool hasValidMagic(cchar *sp,int sl,cchar *ms) noex  {
	return hasvalidmagic(sp,sl,ms) ;
}
static inline bool hasNotDots(cchar *sp,int sl) noex {
	return hasnotdots(sp,sl) ;
}
static inline bool hasEOH(cchar *sp,int sl) noex {
	return haseoh(sp,sl) ;
}

EXTERNC_end


#endif /* HASX_INCLUDE */



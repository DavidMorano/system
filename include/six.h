/* six HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* string-index operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	SIX_INCLUDE
#define	SIX_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	<siext.h>
#include	<sifext.h>
#include	<sisub.h>
#include	<siwht.h>
#include	<sixbrk.h>


EXTERNC_begin

extern int siochr(cchar *,int,int) noex ;
extern int sirchr(cchar *,int,int) noex ;
extern int sicasechr(cchar *,int,int) noex ;
extern int sifield(cchar *,int,int) noex ;

extern int sialpha(cchar *,int) noex ;
extern int sidigit(cchar *,int) noex ;
extern int sialnum(cchar *,int) noex ;
extern int sidquote(cchar *,int) noex ;
extern int sibasename(cchar *,int) noex ;
extern int sinext(cchar *,int) noex ;
extern int siskipwhite(cchar *,int) noex ;
extern int silbrace(cchar *,int) noex ;
extern int sileader(cchar *,int) noex ;
extern int sihyphen(cchar *,int) noex ;

extern int sinonoct(cchar *,int) noex ;
extern int sinondec(cchar *,int) noex ;
extern int sinonhex(cchar *,int) noex ;

extern int sispan(cchar *,int,cchar *) noex ;
extern int sicite(cchar *,int,cchar *,int) noex ;
extern int siterm(cchar *,int,cchar *) noex ;

static inline int sichr(cchar *sp,int sl,int sch) noex {
	return siochr(sp,sl,sch) ;
}
static inline int sibrk(cchar *sp,int sl,cchar *ss) noex {
    	return siobrk(sp,sl,ss) ;
}
static inline int sibreak(cchar *sp,int sl,cchar *ss) noex {
	return sibrk(sp,sl,ss) ;
}

EXTERNC_end


#endif /* SIX_INCLUDE */



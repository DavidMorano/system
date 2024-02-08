/* sfx HEADER */
/* lang=C20 */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SFX_INCLUDE
#define	SFX_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern int sfbasename(cchar *,int,cchar **) noex ;
extern int sfbaselib(cchar *,int,cchar **) noex ;
extern int sfbracketval(cchar *,int,cchar **) noex ;
extern int sfbreak(cchar *,int,cchar *,cchar **) noex ;
extern int sfcasesub(cchar *,int,cchar *,cchar **) noex ;
extern int sfcontent(cchar *,int,cchar **) noex ;
extern int sfcookkey(cchar *,int,cchar **) noex ;
extern int sfchr(cchar *,int,int,cchar **) noex ; /* string following char */
extern int sfdequote(cchar *,int,cchar **) noex ;
extern int sfdirname(cchar *,int,cchar **) noex ;
extern int sfkey(cchar *,int,cchar **) noex ;
extern int sfkeyval(cchar *,int,cchar *,cchar **) noex ;
extern int sflast(cchar *,int,int,cchar **) noex ;
extern int sfnamecomp(cchar *,int,cchar **) noex ;
extern int sfnext(cchar *,int,cchar **) noex ;
extern int sfnextterm(cchar *,int,cchar *,cchar **) noex ;
extern int sfprogroot(cchar *,int,cchar **) noex ;
extern int sfprogname(cchar *,int,cchar **) noex ;
extern int sfrootname(cchar *,int,cchar **) noex ;
extern int sfshrink(cchar *,int,cchar **) noex ;
extern int sfskipwhite(cchar *,int,cchar **) noex ;
extern int sfsub(cchar *,int,cchar *,cchar **) noex ;
extern int sfsubstance(cchar *,int,cchar **) noex ;
extern int sfthing(cchar *,int,cchar *,cchar **) noex ;
extern int sfwhitedot(cchar *,int,cchar **) noex ;
extern int sfword(cchar *,int,cchar **) noex ;
extern int sfsign(bool *,cchar *,int,cchar **) noex ;

static inline int sfbrk(cchar *sp,int sl,cchar *s2,cchar **rpp) noex {
	return sfbreak(sp,sl,s2,rpp) ;
}

static inline int getpr(cchar *pp,int pl,cchar **rpp) noex {
	return sfprogroot(pp,pl,rpp) ;
}

EXTERNC_end

#ifdef	__cplusplus

inline int nextfield(cchar *sp,int sl,cchar **rpp) noex {
	return sfnext(sp,sl,rpp) ;
}

#else /* __cplusplus */

static inline int nextfield(cchar *sp,int sl,cchar **rpp) noex {
	return sfnext(sp,sl,rpp) ;
}

#endif /* __cplusplus */


#endif /* SFX_INCLUDE */



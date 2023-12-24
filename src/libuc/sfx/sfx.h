/* sfx INCLUDE */
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
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int sfshrink(cchar *,int,cchar **) noex ;
extern int sfbasename(cchar *,int,cchar **) noex ;
extern int sfdirname(cchar *,int,cchar **) noex ;
extern int sfbreak(cchar *,int,cchar *,cchar **) noex ;
extern int sfcasesub(cchar *,int,cchar *,cchar **) noex ;
extern int sfdequote(cchar *,int,cchar **) noex ;
extern int sfkey(cchar *,int,cchar **) noex ;
extern int sfprogroot(cchar *,int,cchar **) noex ;
extern int sfprogname(cchar *,int,cchar **) noex ;
extern int sfskipwhite(cchar *,int,cchar **) noex ;
extern int sfsub(cchar *,int,cchar *,cchar **) noex ;
extern int sfthing(cchar *,int,cchar *,cchar **) noex ;
extern int sfwhitedot(cchar *,int,cchar **) noex ;
extern int sfword(cchar *,int,cchar **) noex ;
extern int sfnext(cchar *,int,cchar **) noex ;
extern int sfnextterm(cchar *,int,cchar *,cchar **) noex ;

#ifdef	__cplusplus
}
#endif

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



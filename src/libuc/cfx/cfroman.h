/* cfroman HEADER */
/* charset=ISO8859-1 */
/* lang=C++11 */

/* convert from Roman Numeral to binary integer */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-08-15, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

#ifndef	CFROMAN_INCLUDE
#define	CFROMAN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<stdintx.h>


EXTERNC_begin

extern int cfromani(cchar *,int,int *) noex ;
extern int cfromanl(cchar *,int,long *) noex ;
extern int cfromanll(cchar *,int,longlong *) noex ;

extern int cfromanui(cchar *,int,uint *) noex ;
extern int cfromanul(cchar *,int,ulong *) noex ;
extern int cfromanull(cchar *,int,ulonglong *) noex ;

static inline int cfroman(cchar *sp,int sl,int *rp) noex {
	return cfromani(sp,sl,rp) ;
}

EXTERNC_end

#ifdef	__cplusplus

inline int cfroman(cchar *sp,int sl,long *rp) noex {
	return cfromanl(sp,sl,rp) ;
}
inline int cfroman(cchar *sp,int sl,longlong *rp) noex {
	return cfromanll(sp,sl,rp) ;
}
inline int cfroman(cchar *sp,int sl,uint *rp) noex {
	return cfromanui(sp,sl,rp) ;
}
inline int cfroman(cchar *sp,int sl,ulong *rp) noex {
	return cfromanul(sp,sl,rp) ;
}
inline int cfroman(cchar *sp,int sl,ulonglong *rp) noex {
	return cfromanull(sp,sl,rp) ;
}

#endif /* __cplusplus */


#endif /* CFROMAN_INCLUDE */



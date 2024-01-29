/* strwcmp HEADER */
/* lang=C20 */

/* string-copy variant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRWCMP_INCLUDE
#define	STRWCMP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern int strwcmpx(cchar *,cchar *,int) noex ;

static inline int strwcmp(cchar *bs,cchar *sp,int sl) noex {
	return strwcmpx(bs,sp,sl) ;
}

EXTERNC_end

#ifdef	__cplusplus

inline int strwcmpo(cchar *s1,cchar *s2,int len) noex {
	return strwcmpx(s1,s2,len) ;
}
inline int strwcmpr(cchar *s1,cchar *s2,int len) noex {
	return strwcmpx(s2,s1,len) ;
}

#else /* __cplusplus */

static inline int strwcmpo(cchar *s1,cchar *s2,int len) noex {
	return strwcmpx(s1,s2,len) ;
}
static inline int strwcmpr(cchar *s1,cchar *s2,int len) noex {
	return strwcmpx(s2,s1,len) ;
}

#endif /* __cplusplus */


#endif /* STRWCMP_INCLUDE */



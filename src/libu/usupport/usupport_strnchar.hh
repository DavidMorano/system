/* usupport_strnchar HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find a character in a counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTSTRNCHAR_INCLUDE
#define	USUPPORTSTRNCHAR_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


namespace libu {
    extern char *strnalpha(cchar *,int) noex ;
    extern char *strnalnum(cchar *,int) noex ;
    extern char *strndigit(cchar *,int) noex ;
    extern char *strndigex(cchar *,int) noex ;
    extern char *strnoctal(cchar *,int) noex ;
    extern char *strnblank(cchar *,int) noex ;
    inline char *strndig(cchar *sp,int sl) noex {
	return strndigit(sp,sl) ;
    }
    inline char *strnoct(cchar *sp,int sl) noex {
	return strnoctal(sp,sl) ;
    }
    inline char *strndec(cchar *sp,int sl) noex {
	return strndigit(sp,sl) ;
    }
    inline char *strnhex(cchar *sp,int sl) noex {
	return strndigex(sp,sl) ;
    }
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTSTRNCHAR_INCLUDE */



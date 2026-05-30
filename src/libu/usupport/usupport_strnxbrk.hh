/* usupport_strnxbrk HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find a character in a counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-06-08, David AÂ­DÂ­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTSTRNXBRK_INCLUDE
#define	USUPPORTSTRNXBRK_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


namespace libu {
    extern char *strnobrk(cchar *,int,cchar *) noex ;
    extern char *strnrbrk(cchar *,int,cchar *) noex ;
    inline char *strnbrk(cchar *sp,int sl,cchar *ss) noex {
	return libu::strnobrk(sp,sl,ss) ;
    }
    inline char *strnpbrk(cchar *sp,int sl,cchar *ss) noex {
	return libu::strnobrk(sp,sl,ss) ;
    }
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTSTRNXBRK_INCLUDE */



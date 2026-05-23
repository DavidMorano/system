/* usupport_strnxchr HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find a character in a counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-06-08, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTSTRNXCHR_INCLUDE
#define	USUPPORTSTRNXCHR_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


namespace libu {
    extern char *strnochr(cchar *,int,int) noex ;
    extern char *strnrchr(cchar *,int,int) noex ;
    inline char *strnchr(cchar *sp,int sl,int sch) noex {
	return libu::strnochr(sp,sl,sch) ;
    } /* end subroutine */
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTSTRNXCHR_INCLUDE */



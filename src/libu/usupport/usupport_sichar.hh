/* usupport_sichar HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find a character or white-space */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTSICHAR_INCLUDE
#define	USUPPORTSICHAR_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


namespace libu {
    extern int	sialpha(cchar *,int) noex ;
    extern int	sialnum(cchar *,int) noex ;
    extern int	sidigit(cchar *,int) noex ;
    extern int	sidigex(cchar *,int) noex ;
    extern int	sibinar(cchar *,int) noex ;
    extern int	sioctal(cchar *,int) noex ;
    extern int	siwhite(cchar *,int) noex ;
    extern int	siblank(cchar *,int) noex ;
    inline int	sidig(cchar *sp,int sl) noex {
	return libu::sidigit(sp,sl) ;
    } /* end subroutine */
    inline int	sibin(cchar *sp,int sl) noex {
    	return libu::sibinar(sp,sl) ;
    } /* end subroutine */
    inline int	sioct(cchar *sp,int sl) noex {
    	return libu::sioctal(sp,sl) ;
    } /* end subroutine */
    inline int	sidec(cchar *sp,int sl) noex {
    	return libu::sidigit(sp,sl) ;
    } /* end subroutine */
    inline int	sihex(cchar *sp,int sl) noex {
    	return libu::sidigex(sp,sl) ;
    } /* end subroutine */
    inline int	siwht(cchar *sp,int sl) noex {
    	return libu::siwhite(sp,sl) ;
    } /* end subroutine */
    inline int	siblk(cchar *sp,int sl) noex {
    	return libu::siblank(sp,sl) ;
    } /* end subroutine */
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTSICHAR_INCLUDE */



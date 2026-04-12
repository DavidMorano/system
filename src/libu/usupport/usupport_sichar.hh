/* usupport_sichar HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

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
#include	<clanguage.h>
#include	<usysbase.h>


namespace libu {
    extern int	sialpha(cchar *,int) noex ;
    extern int	sialnum(cchar *,int) noex ;
    extern int	sidigit(cchar *,int) noex ;
    extern int	sidigex(cchar *,int) noex ;
    extern int	sioctal(cchar *,int) noex ;
    extern int	siwhite(cchar *,int) noex ;
    extern int	siblank(cchar *,int) noex ;
    inline int sidig(cchar *sp,int sl) noex {
	return sidigit(sp,sl) ;
    }
    inline int sioct(cchar *sp,int sl) noex {
    	return sioctal(sp,sl) ;
    }
    inline int sidec(cchar *sp,int sl) noex {
    	return sidigit(sp,sl) ;
    }
    inline int sihex(cchar *sp,int sl) noex {
    	return sidigex(sp,sl) ;
    }
    inline int siwht(cchar *sp,int sl) noex {
    	return siwhite(sp,sl) ;
    }
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTSICHAR_INCLUDE */



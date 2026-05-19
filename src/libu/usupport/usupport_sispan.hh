/* usupport_sispan HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find a character or white-space */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTSISPAN_INCLUDE
#define	USUPPORTSISPAN_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


namespace libu {
    extern int	sispan		(cchar *,int,cchar *) noex ;
    extern int	sispanalpha	(cchar *,int) noex ;
    extern int	sispanalnum	(cchar *,int) noex ;
    extern int	sispandigit	(cchar *,int) noex ;
    extern int	sispandigex	(cchar *,int) noex ;
    extern int	sispanoctal	(cchar *,int) noex ;
    extern int	sispanwhite	(cchar *,int) noex ;
    extern int	sispanblank	(cchar *,int) noex ;
    inline int	sispandig	(cchar *sp,int sl) noex {
    	return sispandigit(sp,sl) ;
    }
    inline int	sispanoct	(cchar *sp,int sl) noex {
    	return sispanoctal(sp,sl) ;
    }
    inline int	sispandec	(cchar *sp,int sl) noex {
    	return sispandigit(sp,sl) ;
    }
    inline int	sispanhex	(cchar *sp,int sl) noex {
    	return sispandigex(sp,sl) ;
    }
    inline int	sispanwht	(cchar *sp,int sl) noex {
    	return sispanwhite(sp,sl) ;
    }
    inline int	sispanblk	(cchar *sp,int sl) noex {
    	return sispanblank(sp,sl) ;
    }
    inline int	siskipwhite	(cchar *sp,int sl) noex {
    	return sispanwhite(sp,sl) ;
    }
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTSISPAN_INCLUDE */



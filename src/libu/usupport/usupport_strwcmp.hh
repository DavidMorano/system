/* usupport_strwcmp HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* string-comparison variant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTSTRWCMP_INCLUDE
#define	USUPPORTSTRWCMP_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


namespace libu {
    extern int strwbasecmp(cchar *,cchar *,int) noex ;
    extern int strwcasecmp(cchar *,cchar *,int) noex ;
    extern int strwfoldcmp(cchar *,cchar *,int) noex ;
    inline int strwcmp(cchar *bs,cchar *sp,int sl) noex {
	return libu::strwbasecmp(bs,sp,sl) ;
    } /* end subroutine (strwcmp) */
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTSTRWCMP_INCLUDE */



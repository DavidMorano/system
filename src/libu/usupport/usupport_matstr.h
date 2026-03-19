/* usupport_matstr HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTMATSTR_INCLUDE
#define	USUPPORTMATSTR_INCLUDE
#ifdef	__cplusplus /* (C++ only) */


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


namespace libu {
    extern int matbasestr(mainv,cchar *,int = -1) noex ;
    extern int matcasestr(mainv,cchar *,int = -1) noex ;
    extern int matfoldstr(mainv,cchar *,int = -1) noex ;
    inline int matstr(mainv bs,cchar *sp,int sl = -1) noex {
	return matbasestr(bs,sp,sl) ;
    }
} /* end namespace (libu) */


#endif /* __cplusplus (C++ only) */
#endif /* USUPPORTMATSTR_INCLUDE */



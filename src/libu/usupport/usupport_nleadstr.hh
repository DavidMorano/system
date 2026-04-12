/* usupport_nleadstr HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* match on the leading part of a string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTNLEADSTR_INCLUDE
#define	USUPPORTNLEADSTR_INCLUDE
#ifdef	__cplusplus /* (C++ only) */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


namespace libu {
    extern int nleadbasestr(cchar *,cchar *,int) noex ;
    extern int nleadcasestr(cchar *,cchar *,int) noex ;
    extern int nleadfoldstr(cchar *,cchar *,int) noex ;
    inline int nleadstr(cchar *bs,cchar *sp,int sl) noex {
	return nleadbasestr(bs,sp,sl) ;
    }
} /* end namespace (libu) */


#endif /* __cplusplus (C++ only) */
#endif /* USUPPORTNLEADSTR_INCLUDE */



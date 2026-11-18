/* usupport_comstrx HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* match on the leading part of a string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTCOMSTRX_INCLUDE
#define	USUPPORTCOMSTRX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


namespace libu {
    extern int comstrbc(cchar *,cchar *,int = -1) noex ;
    extern int comstric(cchar *,cchar *,int = -1) noex ;
    extern int comstrfc(cchar *,cchar *,int = -1) noex ;
} /* end namespace (libu) */

namespace libu {
    local inline int comstr(cchar *bs,cchar *sp,int sl = -1) noex {
	return comstrbc(bs,sp,sl) ;
    } /* end */
} /* end namespace (libu) */


#endif /* USUPPORTCOMSTRX_INCLUDE */



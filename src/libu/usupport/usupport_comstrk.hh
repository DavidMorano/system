/* usupport_comstrk HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* match on the leading part of a string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTCOMSTRK_INCLUDE
#define	USUPPORTCOMSTRK_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


namespace libu {
    extern int comstrkbc(cchar *bs,cchar *sp,int sl = -1) noex ;
    extern int comstrkic(cchar *bs,cchar *sp,int sl = -1) noex ;
    extern int comstrkfc(cchar *bs,cchar *sp,int sl = -1) noex ;
} /* end namespace (libu) */

namespace libu {
    inline int comstrk(cchar *bs,cchar *sp,int sl = -1) noex {
    	return comstrkbc(bs,sp,sl) ;
    } /* end */
} /* end namespace (libu) */


#endif /* USUPPORTCOMSTRK_INCLUDE */



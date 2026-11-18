/* usupport_cmpstrw HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* string-comparison variant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTCMPSTRW_INCLUDE
#define	USUPPORTCMPSTRW_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


namespace libu {
    extern int cmpstrwbc(cchar *,cchar *,int) noex ;
    extern int cmpstrwic(cchar *,cchar *,int) noex ;
    extern int cmpstrwfc(cchar *,cchar *,int) noex ;
} /* end namespace (libu) */

namespace libu {
    inline int cmpstrw(cchar *bs,cchar *sp,int sl) noex {
	return cmpstrwbc(bs,sp,sl) ;
    } /* end subroutine (cmpstrw) */
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTCMPSTRW_INCLUDE */



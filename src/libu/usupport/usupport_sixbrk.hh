/* usupport_sixbrk HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find a character or white-space */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTSIXBRK_INCLUDE
#define	USUPPORTSIXBRK_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


namespace libu {
    extern int	siobrk(cchar *,int,cchar *) noex ;
    extern int	sirbrk(cchar *,int,cchar *) noex ;
} /* end namespace (libu) */

namespace libu {
    local inline int sibrk(cchar *sp,int sl,cchar *ss) noex {
    	return siobrk(sp,sl,ss) ;
    } /* end */
} /* end namespace (libu) */


#endif /* USUPPORTSIXBRK_INCLUDE */



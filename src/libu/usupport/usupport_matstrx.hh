/* usupport_matstrx HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
	This was first written to give a little bit to UNIX® what
	we have in our own circuit-pack OSes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTNATSTRX_INCLUDE
#define	USUPPORTNATSTRX_INCLUDE
#ifdef	__cplusplus /* (C++ only) */


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


namespace libu {
    extern int matstrbc		(con mainv,cchar *,int = -1) noex ;
    extern int matstric		(con mainv,cchar *,int = -1) noex ;
    extern int matstrfc		(con mainv,cchar *,int = -1) noex ;
} /* end namespace (libu) */

namespace libu {
    inline int matstr		(con mainv bs,cchar *sp,int sl = -1) noex {
	return matstrbc(bs,sp,sl) ;
    } /* end subroutine */
} /* end namespace (libu) */


#endif /* __cplusplus (C++ only) */
#endif /* USUPPORTNATSTRX_INCLUDE */



/* usupport_matstro HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* USUPPORT Match-String-Option */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTMATSTRO_INCLUDE
#define	USUPPORTMATSTRO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


namespace libu {
    extern int matstrobc(con mainv,int,cchar *,int = -1) noex ;
    extern int matstroic(con mainv,int,cchar *,int = -1) noex ;
    extern int matstrofc(con mainv,int,cchar *,int = -1) noex ;
} /* end namespace (libu) */

namespace libu {
    inline int matstro(con mainv bs,int n,cchar *sp,int sl = -1) noex {
	return matstrobc(bs,n,sp,sl) ;
    } /* end subroutine (matstro) */
} /* end namespace (libu) */


#endif /* USUPPORTMATSTRO_INCLUDE */



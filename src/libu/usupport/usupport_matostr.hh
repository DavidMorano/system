/* usupport_matostr HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* USUPPORT Match-Option-String */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTMATOSTR_INCLUDE
#define	USUPPORTMATOSTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


namespace libu {
    extern int matobasestr(con mainv,int,cchar *,int) noex ;
    extern int matocasestr(con mainv,int,cchar *,int) noex ;
    extern int matofoldstr(con mainv,int,cchar *,int) noex ;
    inline int matostr(con mainv bs,int n,cchar *sp,int sl = -1) noex {
	return libu::matobasestr(bs,n,sp,sl) ;
    } /* end subroutine (matostr) */
} /* end subroutine (libu) */


#endif /* USUPPORTMATOSTR_INCLUDE */



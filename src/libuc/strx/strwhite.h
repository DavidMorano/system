/* strwhite HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find the first white-space character in a c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Notes:
	1. The existence of this subroutine (oringally |strwhite(3uc)|
	is historical.  Although it is not deprecated, it is rarely
	used and other alternatives exist.

*******************************************************************************/

#ifndef	STRWHITE_INCLUDE
#define	STRWHITE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin
extern char *strwht(cchar *,int) noex ;
local inline char *strwhite(cchar *sp) noex {
    	return strwht(sp,-1) ;
}
EXTERNC_end

#if	__cpluspls

inline char *strwhite(cchar *sp,int sl) noex {
    	return strwht(sp,sl) ;
}
inline char *strwit(cchar *sp) noex {
    	return strwht(sp,sl) ;
}

#endif /* __cpluspls */


#endif /* STRWHITE_INCLUDE */



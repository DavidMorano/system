/* strwhite HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* c-string comparisons */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRWHITE_INCLUDE
#define	STRWHITE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin
extern char	*strwhite(cchar *) noex ; /* <- historical */
EXTERNC_end

#if	__cpluspls

inline char *strwhite(cchar *sp) noex {
    	return strwhite(sp,-1) ;
}
inline char *strwht(cchar *sp,int sl) noex {
    	return strwhite(sp,sl) ;
}

#endif /* __cpluspls */


#endif /* STRWHITE_INCLUDE */



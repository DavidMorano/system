/* strwhite HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* c-string comparisons */
/* version %I% last-modified %G% */


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

extern char	*strwhite(cchar *,int) noex ;

local inline char *strwht(cchar *sp,int sl) noex {
    	return strwhite(sp,sl) ;
}

#endif /* __cpluspls */


#endif /* STRWHITE_INCLUDE */



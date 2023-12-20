/* cthexstr INCLUDE */
/* lang=C20 */

/* subroutine to convert a value (as a counted string) to a HEX string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CTHEXSTR_INCLUDE
#define	CTHEXSTR_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

int cthexstring(char *dbuf,int dlen,int f,cchar *sp,int sl) noex ;

static inline int cthexstr(char *dp,int dl,cchar *sp,int sl) noex {
	cint	f = true ;
	return cthexstring(dp,dl,f,sp,sl) ;
}

#ifdef	__cplusplus
}
#endif


#endif /* CTHEXSTR_INCLUDE */



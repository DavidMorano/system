/* strnchar HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find a character in a counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRNCHAR_INCLUDE
#define	STRNCHAR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>

#include	<strnwht.h>


EXTERNC_begin

extern char *strnalpha(cchar *,int) noex ;
extern char *strnalnum(cchar *,int) noex ;
extern char *strndigit(cchar *,int) noex ;
extern char *strndigex(cchar *,int) noex ;
extern char *strnoctal(cchar *,int) noex ;
extern char *strnblank(cchar *,int) noex ;

static inline char *strndig(cchar *sp,int sl) noex {
	return strndigit(sp,sl) ;
}
static inline char *strnoct(cchar *sp,int sl) noex {
	return strnoctal(sp,sl) ;
}
static inline char *strndec(cchar *sp,int sl) noex {
	return strndigit(sp,sl) ;
}
static inline char *strnhex(cchar *sp,int sl) noex {
	return strndigex(sp,sl) ;
}

EXTERNC_end


#endif /* STRNCHAR_INCLUDE */



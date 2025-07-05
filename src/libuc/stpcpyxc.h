/* stpcpyxc HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* copy a c-string to destination buffer w/ case conversions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STPCPYXC_INCLUDE
#define	STPCPYXC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<strcpyxc.h>


EXTERNC_begin

static inline char * stpcpybc (char *dp,cchar *sp) noex {
	return strcpybc(dp,sp) ;
}
static inline char * stpcpylc (char *dp,cchar *sp) noex {
	return strcpylc(dp,sp) ;
}
static inline char * stpcpyuc (char *dp,cchar *sp) noex {
	return strcpyuc(dp,sp) ;
}
static inline char * stpcpyfc (char *dp,cchar *sp) noex {
	return strcpyfc(dp,sp) ;
}

EXTERNC_end


#endif /* STPCPYXC_INCLUDE */



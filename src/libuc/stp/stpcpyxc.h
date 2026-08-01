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
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<strcpyxc.h>		/* LIBUC */


EXTERNC_begin

local inline char * stpcpybc (char *dp,cchar *sp) noex {
	return strcpybc(dp,sp) ;
}
local inline char * stpcpylc (char *dp,cchar *sp) noex {
	return strcpylc(dp,sp) ;
}
local inline char * stpcpyuc (char *dp,cchar *sp) noex {
	return strcpyuc(dp,sp) ;
}
local inline char * stpcpyfc (char *dp,cchar *sp) noex {
	return strcpyfc(dp,sp) ;
}

EXTERNC_end


#endif /* STPCPYXC_INCLUDE */



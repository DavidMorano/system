/* strcpyxc HEADER */
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
#include	<stpcpyxc.h>		/* LIBUC */


EXTERNC_begin

local inline char * strcpybc (char *dp,cchar *sp) noex {
	return stpcpybc(dp,sp) ;
} /* end */
local inline char * strcpylc (char *dp,cchar *sp) noex {
	return stpcpylc(dp,sp) ;
} /* end */
local inline char * strcpyuc (char *dp,cchar *sp) noex {
	return stpcpyuc(dp,sp) ;
} /* end */
local inline char * strcpyfc (char *dp,cchar *sp) noex {
	return stpcpyfc(dp,sp) ;
} /* end */

EXTERNC_end


#endif /* STPCPYXC_INCLUDE */



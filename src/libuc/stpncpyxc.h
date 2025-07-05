/* stpncpyxc HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* string-copy variant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STPNCPYXC_INCLUDE
#define	STPNCPYXC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<strncpyxc.h>


EXTERNC_begin

static inline char * stpncpybc (char *dp,cchar *sp,int n) noex {
	return strncpybc (dp,sp,n) ;
}
static inline char * stpncpylc (char *dp,cchar *sp,int n) noex {
	return strncpylc (dp,sp,n) ;
}
static inline char * stpncpyuc (char *dp,cchar *sp,int n) noex {
	return strncpyuc (dp,sp,n) ;
}
static inline char * stpncpyfc (char *dp,cchar *sp,int n) noex {
	return strncpyfc (dp,sp,n) ;
}

EXTERNC_end


#endif /* STPNCPYXC_INCLUDE */



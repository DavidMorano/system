/* strnxbrk HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find a character in a counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-06-08, David AÂ­DÂ­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	STRNXBRK_INCLUDE
#define	STRNXBRK_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern char *strnobrk(cchar *,int,cchar *) noex ;
extern char *strnrbrk(cchar *,int,cchar *) noex ;

static inline char *strnbrk(cchar *sp,int sl,cchar *ss) noex {
	return strnobrk(sp,sl,ss) ;
}

static inline char *strnpbrk(cchar *sp,int sl,cchar *ss) noex {
	return strnobrk(sp,sl,ss) ;
}

EXTERNC_end


#endif /* STRNXBRK_INCLUDE */



/* strn INCLUDE */
/* lang=C20 */

/* find a character in a counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRN_INCLUDE
#define	STRN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>

#include	<strnxchr.h>
#include	<strncpyxc.h>


#ifdef	__cplusplus
extern "C" {
#endif

/* find */
extern char *strnsub(cchar *,int,cchar *) noex ;
extern char *strncasesub(cchar *,int,cchar *) noex ;
extern char *strnpbrk(cchar *,int,cchar *) noex ;
extern char *strnrpbrk(cchar *,int,cchar *) noex ;

/* copy-set */
extern char *strnset(char *,int,int) noex ;
extern char *strnwcpy(char *,int,cchar *,int) noex ;

static inline char *strnsetblanks(char *dp,int dl) noex {
	return strnset(dp,dl,' ') ;
}

#ifdef	__cplusplus
}
#endif


#endif /* STRN_INCLUDE */



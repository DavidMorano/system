/* strnxchr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find a character in a counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-06-08, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	STRNXCHR_INCLUDE
#define	STRNXCHR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern char *strnochr(cchar *,int,int) noex ;
extern char *strnrchr(cchar *,int,int) noex ;

local inline char *strnchr(cchar *sp,int sl,int sch) noex {
	return strnochr(sp,sl,sch) ;
} /* end */

EXTERNC_end


#endif /* STRNXCHR_INCLUDE */



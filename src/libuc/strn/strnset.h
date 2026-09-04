/* strnset HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* set a string to a number of characters (exactly - not-NUL-terminated) */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	STRNSET_INCLUDE
#define	STRNSET_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern char	*strnset(char *dp,int ch,int n) noex ;

local inline char *strnblanks(char *dp,int n) noex {
	return strnset(dp,' ',n) ;
} /* end */

EXTERNC_end


#endif /* STRNSET_INCLUDE */



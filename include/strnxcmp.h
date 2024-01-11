/* strnxcmp HEADER */
/* lang=C20 */

/* find a character in a counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRNXCMP_INCLUDE
#define	STRNXCMP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


EXTERNC_begin

extern int strnkeycmp(cchar *,cchar *,int) noex ;
extern int strnleadcmp(cchar *,cchar *,int) noex ;
extern int strnncmp(cchar *,int,cchar *,int) noex ;
extern int strnndictcmp(cchar *,int,cchar *,int) noex ;

EXTERNC_end


#endif /* STRNXCMP_INCLUDE */



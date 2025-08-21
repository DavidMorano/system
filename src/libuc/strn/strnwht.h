/* strnwht HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* search for a possible break-point among possiblw hite-space */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRNWHT_INCLUDE
#define	STRNWHT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern char *strnwht(cchar *,int) noex ;
extern char *strnwhtchr(cchar *,int,int) noex ;
extern char *strnwhtbrk(cchar *,int,cchar *) noex ;

EXTERNC_end


#endif /* STRNWHT_INCLUDE */



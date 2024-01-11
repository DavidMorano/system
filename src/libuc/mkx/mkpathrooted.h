/* mkpathrooted HEADER */
/* lang=C20 */

/* make a file rooted from the present-working-directory */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MKPATHROOTED_INCLUDE
#define	MKPATHROOTED_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern int mkpathrooted(char *,cchar *) noex ;

EXTERNC_end


#endif /* MKPATHROOTED_INCLUDE */



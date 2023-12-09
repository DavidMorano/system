/* hasfext INCLUDE */
/* lang=C20 */

/* determine if file-name (just a string) has an approved file-extension */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HASFEXT_INCLUDE
#define	HASFEXT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>

#ifdef	__cplusplus
extern "C" {
#endif

int hasfext(cchar **,cchar *,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* HASFEXT_INCLUDE */



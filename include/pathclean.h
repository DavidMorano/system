/* pathclean INCLUDE */
/* lang=C20 */

/* add a c-string to the end of an existing path c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PATHCLEAN_INCLUDE
#define	PATHCLEAN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int pathclean(char *,cchar *,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* PATHCLEAN_INCLUDE */



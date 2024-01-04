/* getaddr HEADER */
/* lang=C20 */

/* convert a single character to a given case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETADDR_INCLUDE
#define	GETADDR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int getaddrfamily(cchar *) noex ;
extern int getaddrlen(int) noex ;
extern int getaflen(int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* GETADDR_INCLUDE */



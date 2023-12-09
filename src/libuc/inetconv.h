/* inetconv INCLUDE */
/* lang=C20 */

/* is a given c-string a valid INET address (either 4 or 6)? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	INETCONV_INCLUDE
#define	INETCONV_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int inetnetpton(void *,int,int,cchar *,int) noex ;
extern int inetpton(void *,int,int,cchar *,int) noex ;
extern int inetntop(char *,int,int,cvoid *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* INETCONV_INCLUDE */



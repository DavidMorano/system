/* mallocstuff INCLUDE */
/* lang=C20 */

/* miscellaneous |malloc(3c)| related */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MALLOCSTUFF_INCLUDE
#define	MALLOCSTUFF_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

char	*mallocbuf(void *,int) noex ;
char	*mallocstr(const char *) noex ;
char	*mallocstrw(const char *,int) noex ;
char	*malloctest(int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* MALLOCSTUFF_INCLUDE */



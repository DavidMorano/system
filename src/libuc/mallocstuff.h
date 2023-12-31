/* mallocstuff HEADER */
/* lang=C20 */

/* miscellaneous |malloc(3c)| related */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A�D� Morano.  All rights reserved. */

#ifndef	MALLOCSTUFF_INCLUDE
#define	MALLOCSTUFF_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern char	*mallocbuf(void *,int) noex ;
extern char	*mallocstr(cchar *) noex ;
extern char	*mallocstrw(cchar *,int) noex ;
extern char	*malloctest(int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* MALLOCSTUFF_INCLUDE */



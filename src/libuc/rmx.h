/* rmx INCLUDE */
/* lang=C20 */

/* remove EOL (End-Of-Line) characters from the given string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	RMX_INCLUDE
#define	RMX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>

#include	<rmeol.h>		/* <- |rmeol(3uc)| is in there */


#ifdef	__cplusplus
extern "C" {
#endif

extern int	rmchr(cchar *,int,int) noex ;
extern int	rmext(cchar *,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* RMX_INCLUDE */



/* getprogpath INCLUDE */
/* lang=C20 */

/* convert a single character to a given case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETPROGPATH_INCLUDE
#define	GETPROGPATH_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<char.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int getprogpath(ids *,vecstr *,char *,cchar *,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* GETPROGPATH_INCLUDE */



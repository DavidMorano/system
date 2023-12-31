/* quoteshellarg HEADER */
/* lang=C20 */

/* Make-a-Quoted c-string (from a source c-string) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A�D� Morano.  All rights reserved. */

#ifndef	QUOTESHELLARG_INCLUDE
#define	QUOTESHELLARG_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int quoteshellarg(char *,int,cchar *,int,char **) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* QUOTESHELLARG_INCLUDE */



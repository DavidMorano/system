/* sncpy INCLUDE */
/* lang=C20 */

/* counted c-string copy operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	SNCPY_INCLUDE
#define	SNCPY_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<sncpyx.h>
#include	<sncpyxw.h>
#include	<sncpyxc.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int	sncpyarray(char *,int,cchar **,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* SNCPY_INCLUDE */



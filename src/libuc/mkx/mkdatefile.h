/* mkdatefile */
/* lang=C20 */

/* make a temporary date-type job file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	MKDATEFILE_INCLUDE
#define	MKDATEFILE_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int mkdatefile(char *,cchar *,cchar *,mode_t) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* MKDATEFILE_INCLUDE */



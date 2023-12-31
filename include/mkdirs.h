/* mkdirs HEADER */
/* lang=C20 */

/* make a series of possibly hierarchical directories */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A�D� Morano.  All rights reserved. */

#ifndef	MKDIRS_INCLUDE
#define	MKDIRS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int mkdirs(cchar *,mode_t) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* MKDIRS_INCLUDE */



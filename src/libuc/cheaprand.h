/* cheaprand HEADER */
/* lang=C20 */

/* retrieve a cheap random number */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A�D� Morano.  All rights reserved. */

#ifndef	CHEAPRAND_INCLUDE
#define	CHEAPRAND_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern ulong	cheaprand(ulong) noex ;

EXTERNC_end


#endif /* CHEAPRAND_INCLUDE */



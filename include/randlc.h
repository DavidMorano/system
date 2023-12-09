/* randlc INCLUDE */
/* lang=C20 */

/* implement Linear-Congruential random-number generator (RNG) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A�D� Morano.  All rights reserved. */

#ifndef	RANDLC_INCLUDE
#define	RANDLC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int	randlc(int v) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* RANDLC_INCLUDE */



/* powtwo INCLUDE */
/* lang=C20 */

/* handle some power-of-two issues */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A�D� Morano.  All rights reserved. */

#ifndef	POWTWO_INCLUDE
#define	POWTWO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int	nextpowtwo(int) noex ;

static inline int ispowtwo(int v) noex {
	return (v && !(v & (v - 1))) ;
}

#ifdef	__cplusplus
}
#endif


#endif /* POWTWO_INCLUDE */



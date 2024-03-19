/* ipow HEADER */
/* lang=C20 */

/* calculate the integer exponential of an integer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	IPOW_INCLUDE
#define	IPOW_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern int	ipow(int,int) noex ;

static inline int ipowell(int b,int e) noex {
	return ipow(b,e) ;
}

EXTERNC_end


#endif /* IPOW_INCLUDE */


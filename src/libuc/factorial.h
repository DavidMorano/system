/* factorial HEADER */
/* lang=C20 */

/* calculate factorial */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A�D� Morano.  All rights reserved. */

#ifndef	FACTORIAL_INCLUDE
#define	FACTORIAL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<char.h>


EXTERNC_begin

extern long factorial(int) noex ;

EXTERNC_end


#endif /* FACTORIAL_INCLUDE */



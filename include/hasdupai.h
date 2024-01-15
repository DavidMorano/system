/* hasdupai HEADER */
/* lang=C20 */

/* does the given array of integers have duplicate entries */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A�D� Morano.  All rights reserved. */

#ifndef	HASDUPAI_INCLUDE
#define	HASDUPAI_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern int hasdupai(cint *,int) noex ;

EXTERNC_end


#endif /* HASDUPAI_INCLUDE */



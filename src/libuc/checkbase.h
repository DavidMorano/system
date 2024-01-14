/* checkbase HEADER */
/* lang=C20 */

/* convert a single character to a given case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A�D� Morano.  All rights reserved. */

#ifndef	CHECKBASE_INCLUDE
#define	CHECKBASE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern int checkbase(cchar *,int,int) noex ;

EXTERNC_end


#endif /* CHECKBASE_INCLUDE */



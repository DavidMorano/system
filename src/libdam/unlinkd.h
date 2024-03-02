/* unlinkd HEADER */
/* lang=C20 */

/* unlink a file after a specified delay (in seconds) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A�D� Morano.  All rights reserved. */

#ifndef	UNLINKD_INCLUDE
#define	UNLINKD_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern int	unlinkd(cchar *,int) noex ;

EXTERNC_end


#endif /* UNLINKD_INCLUDE */


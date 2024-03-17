/* optval HEADER */
/* lang=C20 */

/* divine a value (boolean or otherwise) out of a c-sring */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	OPTVAL_INCLUDE
#define	OPTVAL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern int	optval(cchar *,int) noex ;

EXTERNC_end


#endif /* OPTVAL_INCLUDE */



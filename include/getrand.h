/* getrand HEADER */
/* lang=C20 */

/* retrieve a cheap random number */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETRAND_INCLUDE
#define	GETRAND_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern int	getrand(void *,int) noex ;

EXTERNC_end


#endif /* GETRAND_INCLUDE */



/* getstacksize HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get a stack size (of some type) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETSTACKSIZE_INCLUDE
#define	GETSTACKSIZE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<stacktypes.h>


EXTERNC_begin

extern int	getstacksize(stacktypes) noex ;

EXTERNC_end


#endif /* GETSTACKSIZE_INCLUDE */



/* rcmdr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get a seed value for use in initializing a true RNG */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This was written from scratch.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	rcmdr

*******************************************************************************/

#ifndef	RCMDR_INCLUDE
#define	RCMDR_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	rcmdr(cchar *,cchar *,cchar *,int *) noex ;

EXTERNC_end


#endif /* RCMDR_INCLUDE */



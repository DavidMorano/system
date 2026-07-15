/* writeto HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* perform a write operation with a timeout */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	WRITETO_INCLUDE
#define	WRITETO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	writeto(int,cvoid *,int,int) noex ;

EXTERNC_end


#endif /* WRITETO_INCLUDE */



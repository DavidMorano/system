/* getnoise HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* retrieve noise from the system */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETNOISE_INCLUDE
#define	GETNOISE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin
extern int	getnoise(uint *,int) noex ;
EXTERNC_end


#endif /* GETNOISE_INCLUDE */



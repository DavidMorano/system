/* dfactorial HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* calculate the factorial of an integer yieling a double floating result */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DFACTORIAL_INCLUDE
#define	DFACTORIAL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern double		dfactorial(int) noex ;
extern long double	ldfactorial(int) noex ;

EXTERNC_end


#endif /* DFACTORIAL_INCLUDE */



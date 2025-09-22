/* factorial HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* calculate (integer) factorial */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	FACTORIAL_INCLUDE
#define	FACTORIAL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern long	factorial(int) noex ;

extern double		dfactorial(int) noex ;
extern long double	ldfactorial(int) noex ;

EXTERNC_end


#endif /* FACTORIAL_INCLUDE */



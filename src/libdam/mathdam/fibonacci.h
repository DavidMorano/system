/* fibonacci HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* calculate (integer) Fibonacci */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	FIBONACCI_INCLUDE
#define	FIBONACCI_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern long	fibonacci(int) noex ;

extern double		dfibonacci(int) noex ;
extern long double	ldfibonacci(int) noex ;

EXTERNC_end


#endif /* FIBONACCI_INCLUDE */



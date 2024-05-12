/* multicombinations SUPPORT */
/* lang=C++20 */

/* n-choose-k function WITH repitition */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	multicombinations

	Description:
	We calculate the multicombinations of the given number
	(n-choose-k function WITH repitition).

	Synopsis:
	long multicombinations(int n,int k) noex

	Arguments:
	n	number of items to choose from
	k	nuber of item to choose WITH repitition

	Returns:
	-	the Fibonacci number of the input

	Notes:
	Two immediate answers:
	a) combinations(n+k-1,k)
	b) combinations(n+k-1,n-1)
	also:
	c) factorial(n+l-1) / ( factorial(k) * factorial(n-1) )

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern long		factorial(int) noex ;
    extern long		combinations(int,int) noex ;
    extern long		permutations(int,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* external subroutines */

long multicombinations(int n,int k) noex {
	return combinations((n + k - 1),k) ;
}
/* end subroutine (multicombinations) */



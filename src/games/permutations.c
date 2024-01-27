/* permutations SUPPORT */
/* lang=C20 */

/* n-P-k function */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-03-01, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	permutations

	Description:
	We calculate the permutations of the given number (without
	repititions).

	Synopsis:
	long permutations(int n,int k)

	Arguments:
	n	number of items to choose from
	k	number of item to choose without repitition

	Returns:
	-	the Fibonacci number of the input


	Notes:

	Two ways to calculate:
	1. nPk = n! / (n-k)!
	2. nPk = repeat k times using iterator i: *= (n-i)!

	Note that for permutions *with* repitition, the result of nPk is simply
		exp(n,k)
	(that is: n raised to the power k).


*******************************************************************************/

#include	<envstandards.h>
#include	<limits.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>


/* external subroutines */


/* external subroutines */

extern long	factorial(int) noex ;


/* forward references */


/* local variables */


/* exported subroutines */

long permutations(int n,int k) noex {
	long		ans = -1 ;
	if ((n >= k) && (k >= 0)) {
	    ans = 1 ;
	    if (k == n) {
		ans = factorial(n) ;
	    } else if (k > 0) {
		for (int i = 0 ; i < k ; i += 1) {
		    ans *= n-- ;
		}
	    }
	} else if (k > n) {
	    ans = 0 ;
	}
	return ans ;
}
/* end subroutine (permutations) */



/* combinations */
/* charset=ISO8859-1 */
/* lang=C20 */

/* n-choose-k function WITHOUT repitition */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-03-01, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	combinations

	Description:
	We calculate the combinations n-C-k *without* repititions.

	Synopsis:
	long combinations(int n,int k)

	Arguments:
	n	number of items to choose from
	k	nuber of item to choose without repitition

	Returns:
	-	the Fibonacci number of the input

	= Notes:
	· Combinations *without* repetition (this subroutine):

	special cases
	+ k == 0 -> ans = 1
	+ k == 1 -> ans = n 
	+ k == n -> ans = 1
	+ k > n  -> ans = 0		(sort of a special case)

	general (classic n-choose-k)

		{     n!      }
		{------------ }
		{ k! · (n-k)! }

	· Note for combinations *with* repitition:

	For combinations *with* repitition the result is:

		 (n-k-1)!	  { n-k-1 }   { n-k-1 }
	        -----------	= {       } = {       }
		k! · (n-1)!	  {   k   }   {  n-1  }

	Combination *with* repitition are also called "multicombinations."
	A subroutine for multicombinations is provied (below) and is:

	Synopsis:
	long multicombinations(int n,int k)

*******************************************************************************/

#include	<envstandards.h>
#include	<climits>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>


/* local namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern long		factorial(int) noex ;
    extern long		permutations(int,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

long combinations(int n,int k) noex {
	long		ans = -1 ;
	if ((n >= 0) && (k >= 0)) {
	    ans = 1 ;
	    if (k == 1) {
		ans = n ;
	    } else if (k == n) {
		ans = 1 ;
	    } else if (k > 0) {
	        clong	num = permutations(n,k) ;
	        clong	den = factorial(k) ;
		if ((num >= 0) && (den > 0)) {
	            ans = num / den ;
		} else {
		    ans = -1 ;
		}
	    }
	} else if (k > n) {
	    ans = 0 ;
	}
	return ans ;
}
/* end subroutine (combinations) */

#ifdef	COMMENT
long multicombinations(int n,int k) noex {
	return combinations((n + k - 1),k) ;
}
/* end subroutine (multicombinations) */
#endif /* COMMENT */



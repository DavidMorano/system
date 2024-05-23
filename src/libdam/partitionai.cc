/* partitionai SUPPORT */
/* lang=C++20 */

/* integer array partitioning function */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-10-04, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	partitionai

	Description:
	Partition an array of integers.

	Synopsis:
	int partitionai(int *a,int al,partpred_f partpred,int v) noex

	Arguments:
	a		array
	al		array length
	partpred	function to evaluate the predicate
	v		value to pass to the predicate function

	Returns:
	-	index of pivot (based from 'ri')

	Notes:
        + everyone has their own! (funny!)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"partitionai.h"


/* local defines */


/* local typedefs */

extern "C" {
typedef int	(*partpred_f)(int,int) noex ;
}


/* external subroutines */

extern "C" {
    extern void	arrswapi(int *,int,int) noex ;
}


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int partitionai(int *a,int al,partpred_f fn,int pv) noex {
	int		last = al ;
	for (int i = 0 ; i < last ; i += 1) {
	    cbool	f = (*fn)(a[i],pv) ;
	    if (! f) {
		arrswapi(a,i--,--last) ;
	    }
	} /* end for */
	return last ;
}
/* end subroutine (partitionai) */



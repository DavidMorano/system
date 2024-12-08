/* partitionai SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

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
	typedef int	(*partitionai_f)(int,int) noex
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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<arrswap.h>
#include	<localmisc.h>

#include	"partitionai.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int partitionai(int *a,int al,partitionai_f fn,int pv) noex {
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



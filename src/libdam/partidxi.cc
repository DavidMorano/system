/* partidxi SUPPORT */
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
	partidxi

	Description:
	Partition an array of integers.

	Synopsis:
	int partidxi(int *a,int first,int last,int idx)

	Arguments:
	a		array
	fist		starting index
	last		over-last index
	idx		index to use as pivot

	Returns:
	pi		resulting pivot index

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<arrswap.h>
#include	<localmisc.h>

#include	"partidxi.h"


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

int partidxi(int *a,int first,int last,int idx) noex {
	cint		pv = a[idx] ;
	int		pi = first ;
	if (idx != (last-1)) {
	    arrswapi(a,idx,(last-1)) ;
	}
	for (int i = first ; i < (last-1) ; i += 1) {
	    if (a[i] < pv) {
		if (i != pi) {
		    arrswapi(a,i,pi) ;
		}
		pi += 1 ;
	    }
	} /* end for */
	if (pi != (last-1)) {
	    arrswapi(a,pi,(last-1)) ;
	}
	return pi ;
}
/* end subroutine (partidxi) */



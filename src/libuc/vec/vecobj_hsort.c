/* vecobj_hsort SUPPORT */
/* lang=C20 */

/* vector object list operations (heapsort) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine extends the VECOBJ object to add the
	capability to get the vector-list heap-sorted rather than
	quick-sorted as would be done normally.  There are rare
	occassions when a heap sort (or some sort other than
	Quick-Sort) is desirable.  Those occassions are generally
	only when a Quick-Sort is known to perform poorly (like
	when all of the data is already sorted).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"vecobj.h"


/* local defines */


/* forward references */


/* local variables */


/* exported subroutines */

int vecobj_hsort(vecobj *op,vecobj_vcmp cmpfunc) noex {
	int		rs = SR_FAULT ;
	if (op && cmpfunc) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = SR_OK ;
	        if (! op->f.issorted) {
	            op->f.issorted = true ;
	            if (op->c > 1) {
	                heapsort(op->va,op->i,cmpfunc) ;
	            }
	        } /* end if (sorting needed) */
		rs = op->c ;
	    } /* end if (open) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecobj_hsort) */



/* vecobj_strsort SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* vector object list operations (string-sort) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	vecobj_strsort

	Description:
	This subroutine extends the VECOBJ object to add the
	capability to get the vector-list heap-sorted rather than
	quick-sorted as would be done normally.  There are rare
	occassions when a heap sort (or some sort other than
	Quick-Sort) is desirable.  Those occassions are generally
	only when a Quick-Sort is known to perform poorly (like
	when all of the data is already sorted).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<vstr.h>		/* |vstrsort(3uc)| */
#include	<localmisc.h>

#include	"vecobj.h"


/* local defines */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int vecobj_strsort(vecobj *op,vecobj_vcf vcmp) noex {
	int		rs = SR_FAULT ;
	if (op && vcmp) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->va) ylikely {
	        if (! op->fl.issorted) {
	            op->fl.issorted = true ;
	            if (op->c > 1) {
			ccharpp		va = ccharpp(op->va) ;
			vstrsort_f	scf = vstrsort_f(vcmp) ;
	                vstrsort(va,op->i,scf) ;
	            }
	        }
	        rs = op->c ;
	    } /* end if (open) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecobj_strsort) */



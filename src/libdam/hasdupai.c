/* hasdupai (Has-Duplicate-Array-Integers) */
/* lang=C99 */

/* does the given array of integers have duplicate entries */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	hasdupa

	Description:
	This subroutine checks if the given array of integers has
	duplicate entries.

	Synopsis:
	int hasdupai(cint *sp,int sl) noex

	Arguments:
	sp		source array of integers
	sl		length of source array

	Returns:
	<0		error
	==0		no duplicates
	==1		found a duplicate entry

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<stdlib.h>		/* <- for |qsort(3c)| */
#include	<string.h>		/* <- for |memset(3c)| */
#include	<usystem.h>
#include	<localmisc.h>

#include	"hasdupai.h"


/* local defines */


/* external subroutines */


/* forwards references */

static int	vcmp(cvoid *,cvoid *) noex ;


/* exported subroutines */

int hasdupai(cint *ap,int al) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f = false ;
	if (ap) {
	    rs = SR_OK ;
	    if (al > 1) {
		cint	esize = sizeof(int) ;
		cint	size = ((al+1)*sizeof(int)) ;
	        int	*aa ;
	        if ((rs = uc_libmalloc(size,&aa)) >= 0) {
	            memcpy(aa,ap,size) ;
	            qsort(aa,al,esize,vcmp) ;
	            for (int i = 1 ; (!f) && (i < al) ; i += 1) {
		        f = (aa[i] == aa[i-1]) ;
		    } /* end for */
	            rs1 = uc_libfree(aa) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (needed more work) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (hasdupai) */


/* local subroutines */

int vcmp(cvoid *v1p,cvoid *v2p) noex {
	cint	*i1p = (cint *) v1p ;
	cint	*i2p = (cint *) v2p ;
	return (*i1p - *i2p) ;
}
/* end subroutine (vcmp) */



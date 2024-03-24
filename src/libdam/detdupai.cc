/* detdupai SUPPORT (Determine-Duplicate-Array-Integers) */
/* lang=C++20 */

/* does the given array of integers have duplicate entries */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-10, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	detdupai

	Description:
	This subroutine checks if the given array of integers has
	duplicate entries.

	Synopsis:
	int detdupai(cint *sp,int sl) noex

	Arguments:
	sp		source array of integers
	sl		length of source array

	Returns:
	==1		found a duplicate entry
	==0		no duplicates
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstdlib>		/* <- for |qsort(3c)| */
#include	<cstring>		/* <- for |memset(3c)| */
#include	<usystem.h>
#include	<localmisc.h>

#include	"detdupai.h"


/* local defines */


/* external subroutines */


/* forwards references */

static int	vcmp(cvoid *,cvoid *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int detdupai(cint *ap,int al) noex {
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
/* end subroutine (detdupai) */


/* local subroutines */

int vcmp(cvoid *v1p,cvoid *v2p) noex {
	cint	*i1p = (cint *) v1p ;
	cint	*i2p = (cint *) v2p ;
	return (*i1p - *i2p) ;
}
/* end subroutine (vcmp) */



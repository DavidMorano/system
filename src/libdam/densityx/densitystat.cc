/* densitystatl SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* calculate the mean and variance on some integral numbers */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-02-16, David A­D­ Morano
	This was originally written for performing some statistical
	calculations for simulation related work.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	densitystatl

	Description:
	This subroutine iplements a Floating Point Mean-Variance
	Calculator (for Arrays of LONGs).  This subroutine will
	calculate the mean and variance for an array of 64-bit
	integer values. The results are floating point values.

	Synopsis:
	int densitystatl(ulong *a,int n,double *mp,double *vp) noex

	Arguments:
	a	array of LONG (64-bit) integers
	n	number of elements in array
	mp	pointer to double to hold the 'mean' result
	vp	pointer to double to hold the 'variance' result

	Returns:
	>=0	OK
	<0	something bad

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

#include	"densitystat.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename T>
int densitystatx(T *a,int n,double *mp,double *vp) noex {
    	int		rs = SR_FAULT ;
	if (a) {
	    rs = SR_INVALID ;
	    if (mp) *mp = 0.0 ;
	    if (vp) *vp = 0.0 ;
	    if (n > 0) {
	        double	s1 = 0.0 ;
	        double	s2 = 0.0 ;
	        T	accprob = 0 ;
		rs = SR_OK ;
	        for (int i = 0 ; i < n ; i += 1) {
	            double	x = double(i) ;
	            double	prob = double(a[i]) ;
	            accprob += a[i] ;
	            s1 += (prob * x) ;
	            s2 += (prob * x * x) ;
	        } /* end for */
	        if (accprob > 0) {
	            double	fnum = double(accprob) ;
	            double	m1 = (s1 / fnum) ;
	            if (mp) {
		        *mp = m1 ;
		    }
	            if (vp) {
	                double	m2 = (s2 / fnum) ;
	                *vp = m2 - (m1 * m1) ;
	            }
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine-template (densitystatx) */


/* local variables */


/* exported variables */


/* exported subroutines */

int densitystati(uint *a,int n,double *mp,double *vp) noex {
    	return densitystatx(a,n,mp,vp) ;
}

int densitystatl(ulong *a,int n,double *mp,double *vp) noex {
    	return densitystatx(a,n,mp,vp) ;
}



/* denpercents SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* calculate certain percentages of accumulation from a density */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-02-16, David A­D­ Morano
	This was originally written for performing some statistical
	calculations for simulation work.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Name:
	denpercentsi
	denpercentsl

	Description:
	This module implements a Floating Point Percentage Accumulation
	Calculator (for Arrays of integers).  This subroutine will
	calculate the mean and variance for an array of 32-bit
	integer values.  The results are floating point values.

	Synopsis:
	int denpercents{x}({x} *a,int n,double *pp) noex

	Arguments:
	{x}	integer-concept: either |uint| (i), or |ulong| (l)
	a	array of (|uint| or |ulong|) integers
	n	number of elements in array
	pp	pointer to array of 100 double float to get results

	Returns:
	>=0	OK
	<0	error (system-return)

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<concepts>
#include	<usystem.h>		/* |sysrets| */
#include	<localmisc.h>


/* local defines */


/* local namespaces */

using std::unsigned_integral ;		/* concept */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

cint		hundred = 100 ;


/* forward references */

template<unsigned_integral T> int denpercentsx(T *a,int n,double *ptab) noex {
    	int		rs = SR_FAULT ;
	if (a && ptab) {
	    rs = SR_NOENT ;
	    for (int j = 1 ; j <= hundred ; j += 1) {
	        ptab[j % hundred] = 0.0 ;
	    }
	    if (n > 0) {
	        double	sum = 0.0 ;
	        double	thresh[hundred] ;
	        T	count = 0 ;
	        rs = SR_OK ;
	        for (int j = 1 ; j <= hundred ; j += 1) {
	            ptab[j % hundred] = double(n) ;
	        }
	        for (int i = 0 ; i < n ; i += 1) {
	            count += a[i] ;
	        }
	        sum = double(count) ;
	        for (int j = 1 ; j <= hundred ; j += 1) {
	            thresh[j % hundred] = sum * 0.01 * double(j) ;
	        }
	        {
	            int		j = 1 ;
	            bool	f_done = false ;
	            for (int i = 0 ; (! f_done) && (i < n) ; i += 1) {
	                cdouble		pb = double(a[i]) ;
	                sum += pb ;
	                while ((! f_done) && (sum >= thresh[j % hundred])) {
	                    ptab[j % hundred] = double(i) ;
	                    j += 1 ;
	                    f_done = (j > hundred) ;
	                } /* end if (reached a threshold) */
	            } /* end for */
	        } /* end block */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (denpercentsx) */


/* local variables */


/* exported variables */


/* exported subroutines */

int denpercentsi(uint *a,int n,double *pp) noex {
    	return denpercentsx(a,n,pp) ;
}

int denpercentsl(ulong *a,int n,double *pp) noex {
    	return denpercentsx(a,n,pp) ;
}



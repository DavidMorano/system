/* nthai SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++98 */

/* find the Nth index-value function (for an array of integers) */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-10-04, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	nthai

	Description:
	Find nth element (by index) of an array of numbers.

	Synopsis:
	int nthai(int *a,int ri,int rl,int n) noex

	Arguments:
	a	array
	ri	low
	rl	high
	n	n-th smallest element

	Returns:
	-	

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>		/* |INT_MAX */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"nthai.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

typedef int	(*partpred_f)(int,int) noex ;


/* external subroutines */

extern "C" int	nthai(int *,int,int,int) noex ;

extern "C" int	partitionai(int *,int,partpred_f,int) noex ;


/* local structures */

namespace {
    struct recurser {
	int		*a ;
	int		al ;
	int		n ;
	recurser(int *aa,int aal,int an) noex : a(aa), al(aal), n(an) { } ;
	int	nrecurse(int,int) noex ;
    } ; /* end struct */
} /* end namespace */


/* forward references */

static int	partpred1(int,int) noex ;
static int	partpred2(int,int) noex ;
static int	getpivot(cint *,int) noex ;


/* local variables */


/* exported subroutines */

int nthai(int *a,int ri,int rl,int n) noex {
	recurser	r(a+ri,rl,(n-ri)) ;
	int		ans = INT_MAX ;
	if ((n >= ri) && (n < (ri+rl))) {
	    ans = r.nrecurse(0,rl) ;
	} /* end if (needed) */
	return ans ;
}
/* end subroutine (nthai) */


/* local subroutines */

int recurser::nrecurse(int ri,int rl) noex {
	int		ans = INT_MAX ;
	if ((n >= 0) && (n < (ri+rl))) {
	    int		pv ;
	    int		pi1, pi2 ;
	    int		new_ri, new_rl ;
	    pv = getpivot(a+ri,rl) ;
	    pi1 = partitionai(a+ri,rl,partpred1,pv) + ri ;
	    pi2 = partitionai(a+pi1,((ri+rl)-pi1),partpred2,pv) + pi1 ;
	    if ((n >= pi1) && (n < pi2)) {
	        ans = a[n] ;
	    } else if (n < pi1) {
		new_ri = ri ;
	  	new_rl = (pi1-ri) ;
	        ans = nrecurse(new_ri,new_rl) ;
	    } else {
		new_ri = pi2 ;
	  	new_rl = ((ri+rl)-pi2) ;
	        ans = nrecurse(new_ri,new_rl) ;
	    }
	} /* end if (valid) */
	return ans ;
}
/* end subroutine (recursoer::nrecurse) */

static int partpred1(int e,int pv) noex {
	return (e < pv) ;
}

static int partpred2(int e,int pv) noex {
	return (e <= pv) ;
}

static int getpivot(cint *a,int al) noex {
	int	pvi = (al/2) ;
	if (pvi == 0) {
	    if (al > 1) pvi = 1 ;
	}
	return a[pvi] ;
} /* end subroutine (getpivot) */



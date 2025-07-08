/* editdistance SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++11 */

/* find the edit-distance between two given strings */
/* version %I% last-modified %G% */


/* revision history:

	= 2013-07-11, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2013 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	editdistance

	Description:
        We find (calculate) the edit-distance to convert one string ('a')
	into abother ('b').
	This is the standard formula for getting the edit distance from one
	string to another, using: change, insert, and delete operations.

	a -> b

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<localmisc.h>

#include	"editdistance.h"

import libutil ;

/* local defines */


/* default name spaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine (template) */
using std::max ;			/* subroutine (template) */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

template<typename T> static void ourswap(T &a,T &b) noex {
	T tmp = a ;
	a = b ;
	b = tmp ;
}
/* end subroutine (ourswap) */


/* forward references */

static int	miner(int,int,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int editdistance(cchar *as,cchar *bs) noex {
	int		rs = SR_FAULT ;
	int		el = 0 ;
	if (as && bs) {
	    int		al = lenstr(as) ;
	    int		bl = lenstr(bs) ;
	    int		ai = 0 ;
	    int		n ;
	    if (al < bl) { /* swap inputs */
	        ourswap(as,bs) ;
	        ourswap(al,bl) ;
	    }
    
	    n = ((bl + 1) * 2) ;
	    if (int *ibuf ; (ibuf = new(nothrow) int [n]) != nullptr) {
	        int	*previous	= (ibuf + (ai++ * (bl + 1))) ;
	        int	*current	= (ibuf + (ai++ * (bl + 1))) ;
	        memclear(ibuf,(n * szof(int))) ;
	        for (int i = 1; i <= al ; i += 1) {
	            current[0] = i ;
		    for (int j = 1 ; j <= bl ; j += 1) {
		        if (as[i-1] == bs[j-1]) {
			    current[j] = previous[j-1] ;
		        } else {
			    cint	a = previous[j-1] ;
			    cint	b = previous[j] ;
			    cint	c = current[j-1] ;
		 	    {
			        cint	m = miner(a,b,c) ;
		                current[j] = (1 + m) ;
			    }
		        }
		    } /* end for */
		    for (int j = 0 ; j <= bl ; j += 1) {
		        previous[j] = current[j] ;
		        current[j] = 0 ;
		    }
	       }  /* end for */
	       el = previous[bl] ;
	       delete [] ibuf ;
	    } else {
	        rs = SR_NOMEM ;
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? el : rs ;
}
/* end subroutine (editdistance) */


/* local subroutines */

static int miner(int a,int b,int c) noex {
	return min(min(a,b),c) ;
}
/* end subroutine (miner) */



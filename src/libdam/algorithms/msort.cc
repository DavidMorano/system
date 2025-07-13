/* msort SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++11 */

/* My-Sort (another implementation of Quick-Sort) */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	name:
	msort

  	Description:
	Our version of Quick Sort (sort of the daddy of all sorts,
	mostly).  Everyone has their own, right?

	Synopsis:
	typedef int (*sortcmp_f)(cvoid *,cvoid *) ;
	void msort(void *base,int nel,int esz,sortcmp_f *cmp)

	Arguments:
	base		pointer to base of array to sort
	nel		number of elements in array
	esz		size in bytes of an array element
	cmp		comparison function

	Returns:
	-		nothing (sorted array in place)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>
#include	<new>			/* |nothrow(3c++) */
#include	<initializer_list>
#include	<utility>
#include	<functional>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<vector>
#include	<list>
#include	<array>
#include	<iostream>
#include	<iomanip>
#include	<usystem.h>
#include	<localmisc.h>

#include	"msort.h"


/* local defines */


/* name-spaces */

using std::nothrow ;			/* type */


/* type-defs */

extern "C" {
    typedef int	(*sortcmp_f)(cvoid *,cvoid *) ;
    typedef int	(*partpred_f)(int,int) ;
}


/* external subroutines */

extern "C" int	msort(void *,int,int,sortcmp_f) ;

extern "C" int	partitionai(int *,int,partpred_f,int) ;

extern "C" cchar	*getourenv(cchar **,cchar *) ;


/* local structures */

namespace {
    struct msort_data {
	char		*base = nullptr ;
	int		esz ;		/* element size */
	int		(*cmpfun)(cvoid *,cvoid *) ;
	char		*tmp = nullptr ;
	char		*pvp = nullptr ;
	msort_data(void *baser,int e,sortcmp_f cmp) : esz(e), cmpfun(cmp) { 
	    base = charp(baser) ;
	    tmp = new(nothrow) char[esz + 1] ;
	    pvp = new(nothrow) char[esz + 1] ;
	} ;
	destruct msort_data() {
	    delete [] tmp ;
	    delete [] pvp ;
	}
	void swap(int i1,int i2) {
	    csize	esize = size_t(esz) ;
	    void	*i1p = (base + (i1 * esz)) ;
	    void	*i2p = (base + (i2 * esz)) ;
 	    memcpy(tmp,i1p,esize) ;
 	    memcpy(i1p,i2p,esize) ;
 	    memcpy(i2p,tmp,esize) ;
	} ;
	int dosort(int,int) ;
	int docmp(int i1,int i2) const {
	    cvoid	*i1p = (base + (i1 * esz)) ;
	    cvoid	*i2p = (base + (i2 * esz)) ;
	    return (*cmpfun)(i1p,i2p) ;
	} ;
	void loadpivot(int i) {
	    csize	esize = size_t(esz) ;
	    cvoid	*ip = (base + (i * esz)) ;
	    memcpy(pvp,ip,esize) ;
	} ;
	int getpivot(int,int) ;
    } ; /* end struct (msort_data) */
} /* end namespace */


/* forward references */

static int	partpred1(int,int) ;
static int	partpred2(int,int) ;


/* local variables */


/* exported variables */


/* exported subroutines */

int msort(void *base,int ne,int esz,sortcmp_f cmp) {
	msort_data	data(base,esz,cmp) ;
	int		rs = SR_OK ;
	data.dosort(0,ne) ;
	return rs ;
}
/* end subroutine (msort) */


/* local subroutines */

int msort_data::dosort(int first,int last) {
	int		ff = false ;
	if ((last - first) == 2) {
	    if (docmp(first,last - 1) > 0) {
		swap(first,(last - 1)) ;
	    }
	    ff = true ;
	} else if ((last - first) > 2) {
	    int		pv ;
	    int		m1, m2 ;
	    ff = true ;
	    pv = getpivot(first,(last - first)) ;
	    int		*a = intp(base) ;
	    m1 = partitionai(a + first,(last - first),partpred1,pv) + first ;
	    m2 = partitionai(a + m1,(last - m1),partpred2,pv) + m1 ;
	    if ((m1 - first) > 1) dosort(first,m1) ;
	    if ((last - m2) > 1) dosort(m2,last) ;
	}
	return ff ;
}
/* end method (msort_data::dosort) */

int msort_data::getpivot(int first,int al) {
	int	pvi = (al/2) ;
	if (pvi == 0) {
	    if (al > 1) pvi = 1 ;
	}
	loadpivot(first + pvi) ;
	return pvi ;
}
/* end method (msort_data::getpivot) */

static int partpred1(int e,int pv) {
	return (e < pv) ;
}

static int partpred2(int e,int pv) {
	return (e <= pv) ;
}



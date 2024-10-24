/* isort SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++98 */

/* looks like an Insertion-Sort implementation */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	isort

	Description:
	This looks like an Insertion-Sort implementation.

	Synopsis:
	typedef int	(*sortcmp_f)(cvoid *,cvoid *) ;
	void isort(void *base,int nelem,int esize,sortcmp_f *cmp)

	Arguments:
	base		pointer to base of array to sort
	nelem		number of elements in array
	esize		size in bytes of an array element
	cmp		comparison function

	Returns:
	-		nothing (sorted array in place)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<new>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"isort.h"


/* local defines */


/* local name-spaces */

using std::nothrow ;			/* constant */


/* local typedefs */

extern "C" {
    typedef int	(*sortcmp_f)(cvoid *,cvoid *) noex ;
}


/* external subroutines */

extern "C" int	isort(void *,int,int,sortcmp_f) noex ;


/* external variables */


/* local structures */


/* forward references */


/* exported variables */


/* exported subroutines */

int isort(void *base,int ne,int esz,sortcmp_f cmp) noex {
    	csize		esize = size_t(esz) ;
	int		rs = SR_FAULT ;
	if (base && cmp) {
	    rs = SR_INVALID ;
	    if ((esz > 0) && (ne > 0)) {
	        char	*arr = charp(base) ;
	        rs = SR_NOMEM ;
	        if (char *key ; (key = new(nothrow) char[esz]) != nullptr) {
	            rs = SR_OK ;
   	            for (int i = 1 ; i < ne ; i += 1) {
		        int	j = (i-1) ;
                        memcpy(key,arr+(i*esz),esz) ;
                        while ((j >= 0) && (cmp(arr+(j*esz),key) > 0)) {
		            memcpy(arr+((j+1)*esz),arr+(j*esz),esize) ;
                            j = (j - 1) ;
                        }
                        memcpy(arr+((j+1)*esize),key,esize) ;
                    } /* end for */
	            delete [] key ;
	        } /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (isort) */



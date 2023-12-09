/* isort */
/* lang=C++98 */


/* revision history:

	= 2000-05-14, David A�D� Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright � 2000 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	Our version of Quick Sort (sort of the daddy of all sorts, mostly).
	Everyone has their own, right?

	Synopsis:
	typedef int	(*sortcmp_t)(const void *,const void *) ;
	void isort(void *base,int nelem,int esize,sortcmp_t *cmp)

	Arguments:
	base		pointer to base of array to sort
	nelem		number of elements in array
	esize		size in bytes of an array element
	cmp		comparison function

	Returns:
	-		nothing (sorted array in place)


*******************************************************************************/


#include	<envstandards.h>
#include	<sys/types.h>
#include	<new>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* type defs */

typedef int	(*sortcmp_t)(const void *,const void *) noex ;


/* name-spaces */

using namespace	std ;


/* external subroutines */

extern "C" int	isort(void *,int,int,sortcmp_t) noex ;


/* local structures */


/* forward references */


/* exported subroutines */

int isort(void *base,int nelem,int esize,sortcmp_t cmp) noex {
	int		rs = SR_NOMEM ;
	char		*arr = (char *) base ;
	char		*key ;
	if ((key = new(nothrow) char[esize]) != nullptr) {
	    rs = SR_OK ;
   	    for (int i = 1 ; i < nelem ; i += 1) {
		int	j = (i-1) ;
                memcpy(key,arr+(i*esize),esize) ;
                while ((j >= 0) && (cmp(arr+(j*esize),key) > 0)) {
		    memcpy(arr+((j+1)*esize),arr+(j*esize),esize) ;
                    j = (j - 1) ;
                }
                memcpy(arr+((j+1)*esize),key,esize) ;
            } /* end for */
	    delete [] key ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (isort) */



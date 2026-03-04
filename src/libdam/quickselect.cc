/* quickselect SUPPORT */
/* charset=ISO8859-1 */
/* lang=C99 */

/* quick-select function */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 1998-09-10, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	quickselect

	Description:
	We create the quickselect order on the given array.

	Synopsis:
	int quickselect(int *a,int low,int high,int k) noex

	Arguments:
	a	array if inegers
	low	low
	hight	high
	k	k-th smallest element

	Returns:
	-	

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<localmisc.h>

#include	"quickselect.h"


/* external subroutines */

extern "C" {
    extern void	arrswapi(int *,int,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

void quickselect(int *a,int low,int n,int k) noex {
	cint		high = (n - 1) ;
	k += 1 ;
	if ((high - low) >= k) {
	    cint	mid = (low + ((high - low) / 2)) ;
	    int		i, j ;
	    int		pivot ;

	    arrswapi(a,mid,high) ;

	    pivot = a[high] ;
	    i = low ;
	    j = high - 1 ;

	    while (j >= i) {
	        if ((a[j] < pivot) && (a[i] > pivot)) {
	            arrswapi(a,i++,j--) ;
	        } else if ((a[j] < pivot) && (a[i] <= pivot)) {
	            i += 1 ;
	        } else if ((a[j] >= pivot) && (a[i] > pivot)) {
	            j -= 1 ;
	        } else if ((a[j] >= pivot) && (a[i] <= pivot)) {
	            i += 1 ;
	            j -= 1 ;
	        }
	    } /* end while */

	    arrswapi(a,i,high) ;

	    if (k >= i) {
	        quickselect(a,(i+1),(high+1),k) ;
	    } else {
	        quickselect(a,low,(i+1),k) ;
	    }

	} /* end if (needed) */
}
/* end subroutine (quickselect) */



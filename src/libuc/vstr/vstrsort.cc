/* vstrsort SUPPORT (V-String-Sort) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* a shell-sort on array of pointers to c-strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/*******************************************************************************
 
  	Name:
	vstrsort

	Description:
	This subroutine performs a shell sort on an array of
	c-strings.

	Sunopsis:
	typedef int (*vstrsort_f)(cchar **,cchar **) noex
	void vstrsort(ccharpp sav,int n,vstrsort_f cmpfun) noex

	Arguments:
	sav		array of pointers to c-strings
	n		number of pointers in array
	cmpfun		function call to do sort

	Returns:
	-		no return

	Notes:
	Some good choices for the |vstrsort_f) function are:
	vstrbasecmp
	vstrcasecmp
	vstrfoldcmp
	vstrkeycmp
	vstrkeydictcmp

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"vstrsort.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local inline void strswap(ccharpp app,int i1,int i2) noex {
    	cchar *tmp = app[i1] ;
	app[i1] = app[i2] ;
	app[i2] = tmp ;
} /* end subroutine (strswap) */


/* local variables */


/* exported variables */


/* exported subroutines */

void vstrsort(cchar **sav,int n,vstrsort_f cmpfun) noex {
	int 		j ; /* used-throughout */
	for (j = 1 ; j <= n ; j *= 2) ;
	for (int m = 2 * j - 1 ; m /= 2 ; ) {
	    int k ;
	    for ((j = 0, k = n - m) ; j < k ; j += 1) {
	        for (int i = j ; i >= 0 ; i -= m) {
	            ccharpp app = (sav + i) ;
	            if (cmpfun((app + m),(app + 0)) >= 0) break ;
		    strswap(app,0,m) ;
	        } /* end for */
	    } /* end for */
	} /* end for */
} /* end subroutine (vstrsort) */



/* maxvecaddr SUPPORT */
/* lang=C++20 */

/* maximum core-address determination for a vector-array of strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	maxvecaddr

	Description:
	Find the maximum address of both the pointer-array and of the
	elements being pointed to by the pointers in the pointer-array.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>

#include	"maxvecaddr.h"


/* local defines */


/* exported variables */


/* exported subroutines */

cchar *maxvecaddr(mainv va) noex {
	cchar	*maxp = nullptr ;
	if (va != nullptr) {
	    cchar	*lp ;
	    int		n{} ;
	    for (n = 0 ; va[n] != nullptr ; n += 1) ;
	    lp = ccharp(va + (n + 1)) ;
	    if (lp > maxp) maxp = lp ;
	    {
		cchar	*sp = va[0] ;
	        for (int i = 1 ; i < n ; i += 1) {
		    if (va[n] > sp) sp = va[n] ;
	        } /* end for */
		if (sp != nullptr) {
		    lp = sp ;
		    lp += (strlen(sp) + 1) ;
		    if (lp > maxp) maxp = lp ;
		}
	    }
	} /* end if */
	return maxp ;
}
/* end subroutine (maxvecaddr) */



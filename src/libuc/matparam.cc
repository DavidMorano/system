/* matparam SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* does the parameter array have a given value? */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-07-19, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

	= 2017-12-16, David A­D­ Morano
	Updated.

*/

/* Copyright © 2000,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	matparam

	Description:
	We search to see if our given value is in the given parameter
	array.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* <- for |SHORT_MIN| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<localmisc.h>

#include	"matparam.h"


/* local defines */


/* external subroutines */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

bool matparam(cshort *pp,int pl,int v) noex {
	bool		f = false ;
	for (int i = 0 ; (i < pl) && (pp[i] >= 0) ; i += 1) {
	    cint	pv = int(pp[i]) ;
	    if (pv == SHORT_MIN) break ;
	    f = (pv == v) ;
	    if (f) break ;
	} /* end for */
	return f ;
}
/* end subroutine (matparam) */



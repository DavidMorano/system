/* matparam SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* does the parameter array have a given value? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

	= 2017-12-16, David A­D­ Morano
	Updated.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	matparam

	Description:
	We search to see if our given value is in the given parameter
	array.

	Synopsis:
	bool matparam(cshort *pp,int pl,int v) noex ;

	Arguments:
	pp		parameter array pointer
	pl		parameter array length
	v		value to match

	Returns:
	true		match yes
	false		match no

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* |SHORT_MIN| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"matparam.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

bool matparam(cshort *pp,int pl,int v) noex {
	bool		f = false ;
	if (pp) ylikely {
	    for (int i = 0 ; (i < pl) && (pp[i] >= 0) ; i += 1) {
	        cint	pv = int(pp[i]) ;
	        if (pv == SHORT_MIN) break ;
	        f = (pv == v) ;
	        if (f) break ;
	    } /* end for */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (matparam) */



/* addsat SUPPORT */
/* lang=C++20 */

/* Integer Add-Saturated */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine provides a saturated add operation.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MIN(3c)| + |INT_MAX(3c)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>

#include	"addsat.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

template<typename T>
T addsatx(T v1,T v2) noex {
	T		vr = (v1+v2) ;
	if (v1 >= 0) {
	    if (v2 >= 0) {
		if (vr < 0) vr = INT_MAX ;
	    }
	} else {
	    if (v2 < 0) {
		if (vr >= 0) vr = INT_MIN ;
	    }
	}
	return vr ;
}
/* end subroutine (addsatx) */



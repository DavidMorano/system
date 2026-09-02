/* intsat SUPPORT (Integer-Saturation) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Integer Saturation (clamping) */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	intsat

	Description:
	I provide some saturated add operations.

	Synopsis:
	int intsati(int v) noex
	int intsatl(long v) noex
	int intsatll(longlong v) noex
	int intsatui(uint v) noex
	int intsatul(ulong v) noex
	int intsatull(ulonglong v) noex

	Returns:
	-	an integer value possibly saturated at min or max limit

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |INT_MIN| + |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<stdintx.h>		/* LIBU */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"intsat.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename T> local int intsatsx(T v) noex {
	int	r = int(v) ;
	if (v > INT_MAX) {
	    r = INT_MAX ;
	} else if (v < INT_MIN) {
	    r = INT_MIN ;
	}
	return r ;
} /* end subroutine-template (intsatsx) */

template<typename UT> local int intsatux(UT v) noex {
	UT	im = UT(INT_MAX) ;
	if (v > im) v = im ;
	return int(v) ;
} /* end subroutine-template (intsatux) */


/* exported variables */


/* exported subroutines */

int intsatl	(long v)	noex {
	return intsatsx(v) ;
} /* end */

int intsatll	(longlong v)	noex {
	return intsatsx(v) ;
} /* end */


int intsatui	(uint v)	noex {
    	return intsatux(v) ;
} /* end */

int intsatul	(ulong v)	noex {
    	return intsatux(v) ;
} /* end */

int intsatull(ulonglong v)	noex {
    	return intsatux(v) ;
} /* end */



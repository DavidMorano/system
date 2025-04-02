/* intsat SUPPORT (Integer-Saturation) */
/* encoding=ISO8859-1 */
/* lang=C++20 */

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
	We provide some saturated add operations.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MIN| + |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<stdintx.h>
#include	<localmisc.h>

#include	"intsat.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename T>
int intsatx(T v) noex {
	int	r = int(v) ;
	if (v > INT_MAX) {
	    r = INT_MAX ;
	} else if (v < INT_MIN) {
	    r = INT_MIN ;
	}
	return r ;
}
/* end subroutine-template (intsatx) */

template<typename UT>
int intsatux(UT v) noex {
	UT	im = UT(INT_MAX) ;
	if (v > im) v = im ;
	return int(v) ;
}
/* end subroutine-template (intsatux) */


/* exported variables */


/* exported subroutines */

int intsatl(long v) noex {
	return intsatx(v) ;
}

int intsato(off_t v) noex {
	return intsatx(v) ;
}

int intsatll(longlong v) noex {
	return intsatx(v) ;
}

int intsatui(uint v) noex {
    	return intsatux(v) ;
}

int intsatul(ulong v) noex {
    	return intsatux(v) ;
}

int intsats(size_t v) noex {
    	return intsatux(v) ;
}

int intsatull(ulonglong v) noex {
    	return intsatux(v) ;
}



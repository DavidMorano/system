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

	We provide some saturated add operations.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MIN| + |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
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
	uint		im = uint(INT_MAX) ;
	if (v > im) v = im ;
	return int(v) ;
}

int intsatul(ulong v) noex {
	ulong		im = ulong(INT_MAX) ;
	if (v > im) v = im ;
	return int(v) ;
}

int intsats(size_t v) noex {
	size_t		im = size_t(INT_MAX) ;
	if (v > im) v = im ;
	return int(v) ;
}

int intsatull(ulonglong v) noex {
	ulonglong	im = ulonglong(INT_MAX) ;
	if (v > im) v = im ;
	return int(v) ;
}



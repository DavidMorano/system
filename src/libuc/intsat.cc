/* intsat SUPPORT (Integer-Saturation) */
/* lang=C++20 */

/* perform a variety of integer saturation addition-subtractions */
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
#include	<sys/types.h>
#include	<climits>
#include	<ucvariables.hh>	/* <- for variable |valuelimit| */
#include	<localmisc.h>

#include	"intsat.h"


/* local defines */

#ifndef	LONGLONG_MIN
#define	LONGLONG_MIN	valuelimit.llmin
#endif

#ifndef	LONGLONG_MAX
#define	LONGLONG_MAX	valuelimit.llmax
#endif

#ifndef	ULONGLONG_MAX
#define	ULONGLONG_MAX	valuelimit.ullmax
#endif


/* local subroutine-templates */

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


/* exported subroutines */

int intsatl(long v) noex {
	return intsatx(v) ;
}

int intsatll(longlong v) noex {
	return intsatx(v) ;
}

int intsatul(ulong v) noex {
	ulong	im = ulong(INT_MAX) ;
	if (v > im) v = im ;
	return int(v) ;
}

int intsatull(ulonglong v) noex {
	ulonglong	im = ulonglong(INT_MAX) ;
	if (v > im) v = im ;
	return int(v) ;
}

int iaddsat(int v1,int v2) noex {
	int		vr = (v1+v2) ;
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
/* end subroutine (iaddsat) */

long laddsat(long v1,long v2) noex {
	long		vr = (v1+v2) ;
	if (v1 >= 0) {
	    if (v2 >= 0) {
		if (vr < 0) vr = LONG_MAX ;
	    }
	} else {
	    if (v2 < 0) {
		if (vr >= 0) vr = LONG_MIN ;
	    }
	}
	return vr ;
}
/* end subroutine (laddsat) */

longlong lladdsat(longlong v1,longlong v2) noex {
	longlong	vr = (v1+v2) ;
	if (v1 >= 0) {
	    if (v2 >= 0) {
		if (vr < 0) vr = LONGLONG_MAX ;
	    }
	} else {
	    if (v2 < 0) {
		if (vr >= 0) vr = LONGLONG_MIN ;
	    }
	}
	return vr ;
}
/* end subroutine (lladdsat) */

uint uaddsat(uint v1,uint v2) noex {
	const uint	m = (~ INT_MAX) ;
	uint		vr = (v1+v2) ;
	if ((v1&m) && (v2&m)) vr = UINT_MAX ;
	return vr ;
}
/* end subroutine (uaddsat) */

ulong uladdsat(ulong v1,ulong v2) noex {
	const ulong	m = (~ LONG_MAX) ;
	ulong		vr = (v1+v2) ;
	if ((v1&m) && (v2&m)) vr = ULONG_MAX ;
	return vr ;
}
/* end subroutine (uladdsat) */

ulonglong ulladdsat(ulonglong v1,ulonglong v2) noex {
	const ulonglong	m = (~ LONGLONG_MAX) ;
	ulonglong	vr = (v1+v2) ;
	if ((v1&m) && (v2&m)) vr = ULONGLONG_MAX ;
	return vr ;
}
/* end subroutine (uladdsat) */



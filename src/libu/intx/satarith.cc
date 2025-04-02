/* satarith SUPPORT (Integer-Saturation) */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* perform a variety of integer saturation addition-subtractions */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	satarith

	Description:
	We provide some saturated add operations.

	Synopsis:
	T {x}addsat(T,T) noex

	Returns:
	-		result

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |{x}_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<stdintx.h>		/* extended integer types */
#include	<localmisc.h>

#include	"satarith.h"


import uvariables ;

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


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

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
/* end subroutine (ulladdsat) */



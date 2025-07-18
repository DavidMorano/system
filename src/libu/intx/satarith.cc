/* satarith SUPPORT (Integer-Saturation) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

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
	I provide some saturated add operations.

	Synopsis:
	template<typename T>
	T {x}addsat(T v1,T v2) noex

	Arguments:
	{x}		type of arguments: i, l, ll, ui, ul, ull
	v1		value to add
	v2		value to add

	Returns:
	-		result

	Implementation-notes:
	Remember that in the C and C++ languages, overflow on signed
	integers is undefined-behavior (UB).  And remember that
	you have to avoid UB before it can happen.  Hence the use
	of |willaddover(3u)|.

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
#include	<willaddover.h>
#include	<localmisc.h>

#include	"satarith.h"

import intminmax ;

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures and expressions */


/* forward references */

template<typename T> static T xsaddsat(T v1,T v2) noex {
    	T		vr = 0 ;
    	if (willaddover(v1,v2)) {
	    if (v1 >= 0) {
	        if (v2 >= 0) {
		    vr = vsmax<T> ;
	        }
	    } else {
	        if (v2 < 0) {
		    vr = vsmin<T> ;
	        }
	    }
	} else {
	    vr = (v1 + v2) ;
	}
	return vr ;
}
/* end subroutine (xsaddsat) */

template<typename T> static T xuaddsat(T v1,T v2) noex {
	T		vr = (v1 + v2) ;
	if ((vr < v1) || (vr < v2)) vr = vumax<T> ;
	return vr ;
}
/* end subroutine (xuaddsat) */


/* local variables */


/* exported variables */


/* exported subroutines */

int iaddsat(int v1,int v2) noex {
    	return xsaddsat(v1,v2) ;
}
/* end subroutine (iaddsat) */

long laddsat(long v1,long v2) noex {
    	return xsaddsat(v1,v2) ;
}
/* end subroutine (laddsat) */

longlong lladdsat(longlong v1,longlong v2) noex {
    	return xsaddsat(v1,v2) ;
}
/* end subroutine (lladdsat) */

uint uiaddsat(uint v1,uint v2) noex {
	return xuaddsat(v1,v2) ;
}
/* end subroutine (uaddsat) */

ulong uladdsat(ulong v1,ulong v2) noex {
	return xuaddsat(v1,v2) ;
}
/* end subroutine (uladdsat) */

ulonglong ulladdsat(ulonglong v1,ulonglong v2) noex {
	return xuaddsat(v1,v2) ;
}
/* end subroutine (ulladdsat) */



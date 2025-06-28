/* willaddover SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* will an addition (long integer) overflow? */
/* version %I% last-modified %G% */


/* revision history:

	= 2012-11-21, David A­D­ Morano
	I took this from some of my previous code.

*/

/* Copyright � 2011 David A-D- Morano.  All rights reserved. */

/******************************************************************************

	Name:
	willAddOver

	Description:
	This generic subroutine will determine if an addition will
	overflow.

	Synopsis:
	template<typename T>
	int willaddover(T n1,T n2) noex

	Arguments:
	n1		first number to add
	n2		second number to add

	Returns:
	0 (false)	will not overflow
	1 (true)	will overflow

******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* |LONG_MAX| + |LONG_MIN| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<stdintx.h>
#include	<localmisc.h>

#include	"willaddover.h"

import intminmax ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename T> bool willaddoverx(T n1,T n2) noex {
	bool	f = false ;
	if (n1 > 0) {
	    f = ((n2 > 0) && (n1 > (vsmax<T> - n2))) ;
	} else if (n1 < 0) {
	    f = ((n2 < 0) && (n1 < (vsmin<T> - n2))) ;
	}
	return f ;
}
/* end subroutine (willaddoverx) */


/* local variables */


/* exported variables */


/* exported subroutines */

bool willaddoveri(int n1,int n2) noex {
    	return willaddoverx(n1,n2) ;
}
/* end subroutine (willaddoveri) */

bool willaddoverl(long n1,long n2) noex {
    	return willaddoverx(n1,n2) ;
}
/* end subroutine (willaddoverl) */

bool willaddoverll(longlong n1,longlong n2) noex {
    	return willaddoverx(n1,n2) ;
}
/* end subroutine (willaddoverll) */



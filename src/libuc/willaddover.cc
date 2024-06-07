/* willaddover SUPPORT */
/* lang=C++20 */

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
	int willAddOver(long int n1,long int n2) noex

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
#include	<uvariables.hh>
#include	<localmisc.h>

#include	"willaddover.h"


/* local defines */


/* exported variables */


/* exported subroutines */

bool willaddoveri(int n1,int n2) noex {
	bool	f = false ;
	if (n1 > 0) {
	    f = ((n2 > 0) && (n1 > (INT_MAX - n2))) ;
	} else if (n1 < 0) {
	    f = ((n2 < 0) && (n1 < (INT_MIN - n2))) ;
	}
	return f ;
}
/* end subroutine (willaddoveri) */

bool willaddoverl(long n1,long n2) noex {
	bool	f = false ;
	f = f || ((n1 > 0) && (n2 > 0) && (n1 > (LONG_MAX - n2))) ;
	f = f || ((n1 < 0) && (n2 < 0) && (n1 < (LONG_MIN - n2))) ;
	return f ;
}
/* end subroutine (willaddoverl) */

bool willaddoverll(longlong n1,longlong n2) noex {
	bool f = false ;
	f = f || ((n1 > 0) && (n2 > 0) && (n1 > (valuelimit.llmax - n2))) ;
	f = f || ((n1 < 0) && (n2 < 0) && (n1 < (valuelimit.llmin - n2))) ;
	return f ;
}
/* end subroutine (willaddoverll) */



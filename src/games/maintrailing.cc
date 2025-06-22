/* maintrailing SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++11 */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 2013-07-11, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2013 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	mailtrailering

	Description:
	Fun and games.  We find the nunber of trailing zero digits
	(in decimal) for a given factorial value (itself computed
	from a given source).

	Keywords:
	fives, zeros, twos, tailing, digits

	Synopsis:
	$ trailing

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cinttypes>
#include	<new>
#include	<initializer_list>
#include	<utility>
#include	<functional>
#include	<algorithm>
#include	<vector>
#include	<string>
#include	<fstream>
#include	<iostream>
#include	<iomanip>
#include	<usystem.h>
#include	<localmisc.h>


/* name-spaces */

using namespace std ;


/* external subroutines */

extern "C" uint	factorial(uint) ;

extern "C" int	sisub(cchar *,int,cchar *) ;


/* external variables */


/* local structures (and methods) */


/* forward references */

static int trailing(long) ;


/* local variables */


/* exported subroutines */

int main(int,mainv,mainv) {
	clong		nmax = 30 ;
	for (long n = 0 ; n < nmax ; n += 1) {
	    cint	z = trailing(n) ;
	    cout << "n=" << n << " z=" << z << endl ;
	}
	return 0 ;
}
/* end subroutine (main) */


/* local subroutines */

static int trailing(long n) {
	long	v = n ;
	int		c = 0 ;
	while (v >= 5) {
	    long	t = (v/5) ;
	    c += t ;
	    v = t ;
	}
	return c ;
}
/* end subroutine (trailing) */



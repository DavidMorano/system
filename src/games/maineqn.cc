/* maineqn SUPPORT */
/* lang=C++11 */


/* revision history:

	= 2010-07-11, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2010 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>
#include	<sys/types.h>
#include	<cstdlib>
#include	<cinttypes>
#include	<cstring>
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
#include	<strwcpy.h>
#include	<six.h>
#include	<localmisc.h>


/* name-spaces */

using namespace std ;


/* external subroutines */


/* global variables */


/* local structures (and methods) */


/* forward references */

static int around(double e,double v) ;


/* local variables */


/* exported subroutines */

int main(int argc,mainv,mainv) {
	const double	ext = 6.0 ;
	double		inc = 0.001 ;
	for (double x = 3.0 ; x < +ext ; x += inc) {
	    for (double y = 4.0 ; y < +ext ; y += inc) {
	        double	e1 = (y*y - x*x) ;
	        if (around(e1,10.0)) {
	            double	xy = (x*y) ;
	            cout << "x=" << x << " y=" << y << endl ;
	            cout << "xy=" << xy << endl ;
	        }
	    } /* end for */
	} /* end for */
	return 0 ;
}
/* end subroutine (main) */


/* local subroutines */

static int around(double e,double v) {
	double	epsilon = 0.1 ;
	return ((e >= (v-epsilon)) && (e < (v+epsilon))) ;
}
/* end subroutine (around) */



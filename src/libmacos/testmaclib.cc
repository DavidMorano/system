/* testmaclib SUPPORT */
/* lang=C++11 */


/* revision history:

	= 2010-07-11, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2010 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This code tests the use of the VECINT object.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<cstdlib>
#include	<cstring>
#include	<new>
#include	<algorithm>
#include	<vector>
#include	<string>
#include	<fstream>
#include	<iostream>
#include	<iomanip>
#include	<vecint.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* name-spaces */

using namespace std ;


/* external subroutines */


/* global variables */


/* local structures (and methods) */


/* forward references */


/* local variables */

constexpr int	vals[] = { 3, 7, 19, 12, 43 } ;


/* exported variables */


/* exported subroutines */

int main(int argc,mainv,mainv) {
	vecint		vi ;
	int		rs ;
	int		rs1 ;
	if ((rs = vecint_start(&vi,5,0)) >= 0) {
	    cint	n = nelem(vals) ;
	    if ((rs = vecint_addlist(&vi,vals,n)) >= 0) {
		if ((rs = vecint_assign(&vi,8,71)) >= 0) {
	            int		v ;
	            for (int i = 0 ; vecint_getval(&vi,i,&v) >= 0 ; i += 1) {
		        cout << " " << v ;
	            }
	            cout << endl ;
	        }
	    }
	    vecint_finish(&vi) ;
	} /* end if (vecint) */
	return 0 ;
}
/* end subroutine (main) */


/* local subroutines */



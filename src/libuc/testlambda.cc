/* testlambda SUPPORT */
/* C++20 */

/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

#include	<envstandards.h>	/* ordered first to configure */
#include	<iostream>


/* local defines */

#ifndef	SR_OK
#define	SR_OK	0
#endif

#ifndef	SR_LOCKLOST
#define	SR_LOCKLOST	-20
#endif

#ifndef	SR_TIMEDOUT
#define	SR_TIMEDOUT	-21
#endif


/* external subroutines */

extern "C" {
	static int	msleep(int) ;
}


/* external variables */

volatile sig_atomic_t	f_init = 0 ;
volatile sig_atomic_t	f_initdone = 0 ;


using namespace	std ;

class timewatch {
	int	to ;
public:
	timewatch(int a) : to(a) { } ;
	template<typename L>
	int operator () (L lamb) {
	    int		rs = SR_OK ;
	    int		i = 0 ;
	    for (i = 0 ; (rs == 0) && (i < to) ; i += 1) {
		if ((rs = msleep(1)) >= 0) {
		    rs = lamb() ;
		}
	    } /* end for */
	    if (i == to) rs = SR_TIMEDOUT ;
	    return rs ;
	} ;
} ; /* end class (timewatch) */


/* exported subroutines */

int main(int,const char **,const char **) {
	int	rs = SR_OK ;
	cout << "Hello world!" << '\n' ;
	{
	    timewatch	tw(10) ;
	    auto lamb = [] () -> int {
	        int		rs = SR_OK ;
	        if (!f_init) {
		    rs = SR_LOCKLOST ;
	        } else if (f_initdone) {
		    rs = 1 ;
	        }
	        return rs ;
	    } ;
	    rs = tw(lamb) ;
	} /* end block */
	cout << "exiting rs=" << rs << '\n' ;
	return (rs >= 0) ? 1 : rs ;
}
/* end subroutine (main) */


/* local subroutines */

int init() {
	return 0 ;
}
/* end subroutine (init) */

static int msleep(int to) {
	return 0 ;
}



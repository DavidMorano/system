/* itimerval SUPPORT (Internval-Timer-Value) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® ITIMERVAL object initialization */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	itimerval_load

	Description:
	These subroutines manipulate ITIMERVAL objects.

	Synopsis:
	int itimerval_load(ITIMERVAL *tsp,time_t sec,long nsec) noex

	Arguments:
	tsp		pointer to ITIMERVAL
	sec		seconds
	nsec		nanoseconds

	Returns:
	>=0		OK
	<0		error (system-return)

	Comments:
	typedef struct itimerval {		
		struct timespec	it_interval;	
		struct timespec	it_value;	
	} itimerval_t ;

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<algorithm>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"itimerval.h"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int itimerval_load(ITIMERVAL *tsp,CTIMEVAL *valp,CTIMEVAL *ivp) noex {
	int		rs = SR_FAULT ;
	if (tsp) ylikely {
	    rs = memclear(tsp) ;
	    if (valp) {
	        tsp->it_value = *valp ;
	    }
	    if (ivp) {
	        tsp->it_interval = *ivp ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (itimerval_load) */

const itimerval operator - (con itimerval &v1,con itimerval &v2) noex {
    	itimerval res{} ;
	res.it_value	= v1.it_value - v2.it_value ;
	res.it_interval	= v1.it_interval - v2.it_interval ;
    	return res ;
} /* end subroutine (itimerval::operator) */

const itimerval operator - (con itimerval &v1,con time_t t) noex {
    	itimerval res = v1 ;
	res.it_value.tv_sec -= t ;
    	return res ;
} /* end subroutine (itimerval::operator) */

bool operator == (con itimerval &v1,con itimerval &v2) noex {
    	return (v1.it_value == v2.it_value) ;
} /* end subroutine itimerval::operator) */

ordcmp_weak operator <=> (con itimerval &v1,con itimerval &v2) noex {
    	return (v1.it_value <=> v2.it_value) ;
} /* end subroutine (itimerval::operator) */

bool operator == (con itimerval &itv,int t) noex {
	return (itv.it_value == t) ;
} /* end subroutine */

ordcmp_weak operator <=> (con itimerval &itv,int t) noex {
    	return (itv.it_value <=> t) ;
} /* end subroutine */

bool operator == (con itimerval &itv,time_t t) noex {
	return (itv.it_value == t) ;
} /* end subroutine */

ordcmp_weak operator <=> (con itimerval &itv,time_t t) noex {
    	return (itv.it_value <=> t) ;
} /* end subroutine */



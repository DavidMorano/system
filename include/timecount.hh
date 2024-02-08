/* timecount HEADER */
/* lang=C++20 */

/* count-down timer */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

	= 2011-02-23, David A­D­ Morano
	I updated this to be able to accept lambdas (w/ C++11)

*/

/* Copyright © 2001,2011 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

	This object provides a florified count-down timer.

*******************************************************************************/

#ifndef	TIMECOUNT_INCLUDE
#define	TIMECOUNT_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<utypedefs.h>
#include	<usysrets.h>
#include	<clanguage.h>
#include	<msleep.h>
#include	<localmisc.h>


#ifndef	POLLMULT
#define	POLLMULT	1000
#endif


class timecount {
	int		to ;
public:
	timecount(int a) : to(a*POLLMULT) { } ;
	timecount(const timecount &) = delete ;
	timecount &operator = (const timecount &) = delete ;
	operator int () noex {
	    int		rs = SR_TIMEDOUT ;
	    int		f = true ;
	    if (to-- > 0) {
		f = false ;
		if ((rs = msleep(1)) > 0) {
		    rs = SR_OK ;
		}
	    } /* end if (counting down) */
	    return (rs >= 0) ? f : rs ;
	} ; /* end method (operator) */
} ; /* end class (timecount) */


#endif /* __cplusplus */
#endif /* TIMECOUNT_INCLUDE */



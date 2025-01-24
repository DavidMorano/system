/* timewatch HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* watch time and variables simultaneously */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

	= 2011-02-23, David A­D­ Morano
	I updated this to be able to accept lambdas (w/ C++11)

*/

/* Copyright © 2001,2011 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	timewatch

	Description:
	This simple object provides a facility to watch variables
	for changes.

*******************************************************************************/

#ifndef	TIMEWATCH_INCLUDE
#define	TIMEWATCH_INCLUDE
#ifdef	__cplusplus /* everything is C++ only */


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usupport.h>		/* |msleep(3u)| */


#ifndef	POLL_INTMULT
#define	POLL_INTMULT	1000
#endif


class timewatch {
	int		mto ;		/* millisecond-time-out */
public:
	timewatch(int a) : mto(a * POLL_INTMULT) { } ;
	timewatch(const timewatch &) = delete ;
	timewatch &operator = (const timewatch &) = delete ;
	template<typename L>
	int operator () (L &lamb) noex {
	    int		rs = SR_OK ;
	    bool	fok = true ;
	    for (int i = 0 ; (rs == 0) && (fok = (i < mto)) ; i += 1) {
		if ((rs = msleep(1)) >= 0) {
		    rs = lamb() ;
		}
	    } /* end for */
	    if ((rs == 0) && (! fok)) rs = SR_TIMEDOUT ;
	    return rs ;
	} ; /* end method (operator) */
} ; /* end class (timewatch) */


#endif	/* __cplusplus */
#endif /* TIMEWATCH_INCLUDE */



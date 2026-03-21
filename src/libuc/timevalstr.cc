/* timevalstr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* convert UNIX time into a Julian like character string */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-08-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	timevalstr_ulog

	Description:
	Create a log-oriented time string from a given TIMEVAL value.

	Retrurns:
	-	the pointer to the buffer that was passed by the caller

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<ulogerror.h>
#include	<usysutility.hh>	/* |snprintf(3u)| */
#include	<uctimeconv.h>		/* |uctimelocal(3uc)| */
#include	<localmisc.h>		/* |TIMEBUFLEN| */

#include	"timevalstr.h"


/* local defines */

#ifndef	NYEARS_CENTURY
#define	NYEARS_CENTURY		100
#endif


/* local namespaces */

using libu::snprintf ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		tlen = TIMEBUFLEN ;

/* exported variables */


/* exported subroutines */

char *timevalstr_ulog(TIMEVAL *tvp,char *tbuf) noex {
	TM		*timep = nullptr ;
	if (tvp && tbuf) {
	    int	rs ; ;
	    custime secs = tvp->tv_sec ;
	    if (TM ts ; (rs = uc_timelocal(&secs,&ts)) >= 0) {
	        /* this (below) reallly cannot fail */
	        cchar *fmt = "%02u%02u%02u_%02u%02u:%02u.%06u" ;
	        snprintf(tbuf,tlen,fmt,
	            (timep->tm_year % NYEARS_CENTURY),
	            (timep->tm_mon + 1),
	            timep->tm_mday,
	            timep->tm_hour,
	            timep->tm_min,
	            timep->tm_sec,
	            tvp->tv_usec) ;
	    } else {
		ulogerror("timevalstr_ulog",rs,"timelocal") ;
	    } /* end if (uc_timelocal) */
	} /* end if (non-null) */
	return (timep) ? tbuf : nullptr ;
}
/* end subroutine (timevalstr_ulog) */



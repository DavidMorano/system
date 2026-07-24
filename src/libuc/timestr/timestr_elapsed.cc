/* timestr_elapsed SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* convert UNIX time into an elapsed time character string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	timestr_elapsed

	Description:
	This subroutine creates a string of elapsed time.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>			/* |time_t| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usupport.h>		/* |ulogerror(3u)| */
#include	<snx.h>
#include	<localmisc.h>		/* |TIMEBUFLEN| */

#include	"timestr.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct timepieces {
	char	*rbuf ;
	int	rlen ;
	int	mins ;
	int	secs ;
	int	hours ;
	int	days ;
	timepieces(char *b,int l) noex : rbuf(b), rlen(l) { } ;
	int cvt(cchar *) noex ;
    } ; /* end struct (timepieces) */
} /* end namespace */


/* forward subroutines */


/* local variables */


/* exported variables */


/* exported subroutines */

char *timestr_elapsed(time_t t,char *rbuf) noex {
	cint		rlen = TIMEBUFLEN ;
	int		rs = SR_FAULT ;
	if (rbuf) ylikely {
	    rs = SR_DOM ;
	    if (t >= 0) ylikely {
		timepieces te(rbuf,rlen) ;
	        cuint	tmins = uint(t / 60) ; /* conversion */
	        te.secs = uint(t % 60) ; /* conversion */
		{
	            cuint thours = (tmins / 60) ;
	            te.mins = (tmins % 60) ;
		    {
	                te.days = (thours / 24) ;
	                te.hours = (thours % 24) ;
	                cchar fmt[] = "%5u-%02u:%02u:%02u" ;
	                rs = te.cvt(fmt) ;
		    }
		}
	    } /* end if (valid) */
	    if (rs < 0) rbuf[0] = '\0' ;
	} /* end if (non-null) */
	if (rs < 0) {
	    ulogerror("timestr",rs,"elapsed") ;
	}
	return (rs >= 0) ? rbuf : nullptr ;
} /* end subroutine (timestr_elapsed) */


/* local subroutines */

int timepieces::cvt(cchar *fmt) noex {
    return snwprintf(rbuf,rlen,fmt,days,hours,mins,secs) ;
} /* end method (timepieces::cvt) */



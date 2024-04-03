/* timestr_elapsed SUPPORT */
/* lang=C++20 */

/* convert UNIX time into an elapsed time character string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine creates a string of elapsed time.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<tzfile.h>
#include	<ctime>
#include	<cstdio>
#include	<usystem.h>
#include	<usupport.h>		/* |ulogerror(3u)| */
#include	<snx.h>
#include	<localmisc.h>		/* |TIMEBUFLEN| */

#include	"timestr.h"


/* local defines */


/* external subroutines */


/* forward subroutines */


/* exported variables */


/* exported subroutines */

char *timestr_elapsed(time_t t,char *rbuf) noex {
	cint		rlen = TIMEBUFLEN ;
	int		rs = SR_FAULT ;
	if (rbuf) {
	    rs = SR_DOM ;
	    if (t >= 0) {
	        cuint	tmins = (t / 60) ;
	        cuint	secs = (t % 60) ;
		{
	            cuint	thours = (tmins / 60) ;
	            cuint	mins = (tmins % 60) ;
		    {
	                cuint	days = (thours / 24) ;
	                cuint	hours = (thours % 24) ;
	                cchar	*fmt = "%5u-%02u:%02u:%02u" ;
	                rs = snwprintf(rbuf,rlen,fmt,days,hours,mins,secs) ;
		    }
		}
	    } /* end if (valid) */
	    if (rs < 0) rbuf[0] = '\0' ;
	} /* end if (non-null) */
	if (rs < 0) {
	    ulogerror("timestr",rs,"elapsed") ;
	}
	return (rs >= 0) ? rbuf : nullptr ;
}
/* end subroutine (timestr_elapsed) */



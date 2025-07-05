/* timestr_scandate SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* convert UNIX® time into a VMAIL "scan" date-string format */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-01, David A­D­ Morano
	This code was originally written.  This subroutine
	serves to create the date-string that is expected to be
	seen in the msg-scan line of the display.  I have to provide
	the msg-date-string that the program has previously displayed.

	= 2001-08-23, David A­D­ Morano
	I changed the order of elements in the displayed date.  The
	old date-string was organized to be rather consistent with
	the stupid way that RFC-822 dates are represented.  That
	was always a stupid order to be presented to human beings.

*/

/* Copyright © 1998,2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************
 
  	Name:
	timestr_scandate

	Description:
	Return a date string in the supplied buffer in a format
	(the new format as of 2001-08-23) as (for example):
		 4 Jun 20:52 97

	This amounts to using a |sntmtime(3dam)| format string of:
	    	"%e %b %R %y"

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>			/* |time_t| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<usupport.h>		/* |ulogerror(3u)| */
#include	<tmtime.hh>
#include	<sntmtime.h>
#include	<localmisc.h>		/* |TIMEBUFLEN| */

#include	"timestr.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

char *timestr_scandate(time_t t,char *tbuf) noex {
	cint		tlen = TIMEBUFLEN ;
	int		rs = SR_FAULT ;
	if (tbuf) {
	    rs = SR_DOM ;
	    if (t >= 0) {
	        if (TMTIME ts ; (rs = tmtime_localtime(&ts,t)) >= 0) {
		    cchar	*fmt = "%e %b %R %y" ;
	            rs = sntmtime(tbuf,tlen,&ts,fmt) ;
	        }
	    } /* end if (valid) */
	    if (rs < 0) tbuf[0] = '\0' ;
	} /* end if (non-null) */
	if (rs < 0) {
	    ulogerror("timestr",rs,"scandate") ;
	}
	return (rs >= 0) ? tbuf : nullptr ;
}
/* end subroutine (timestr_scandate) */



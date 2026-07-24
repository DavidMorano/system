/* timestr_date SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* convert UNIX® time into a various date formats */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-01, David A­D­ Morano
	This code was originally written.

	= 2013-11-24, David A­D­ Morano
	I changed this (after all of these years) to use |sntmtime(3dam)|
	rather than |snwprintf(3dam)|.  This should be faster than
	before -- at least that was what was expected.  The
	|sntmtime(3dam)| subroutine is similar to the |strftime(3c)|
	subroutine except that it has a couple of extra format codes
	to make creating time-strings with time-zone offsets within
	them a little bit easier.

*/

/* Copyright © 1998,2013 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	timestr_date
 
	Description:
	Return the date (in UNIX® mail envelope format) into the
	user supplied buffer.

	The correct (newer) UNIX® mail envelope format time string is:

		Wed Jun  4 20:52:47 EDT 1997

	The old UNIX® mail envelope format time string was:

		Wed Jun  4 20:52:47 1997

	Additional note: Even newer UNIX® systems use:

		Wed Jun  4 20:52:47 EDT 1997 -0400

	The program '/usr/lib/mail.local' uses the *old* format
	while the newer program '/usr/bin/mail' uses the new format.
	Most PCS utilities use the newer (newest) format (which is
	(far) suprerior since it includes the timezone abbreviation
	and the time-zone offset value).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |ulogerror(3u)| */
#include	<snx.h>			/* LIBUC |sntmtime(3uc)| */
#include	<sncpyx.h>		/* LIBUC */
#include	<tmtime.hh>		/* LIBUC */
#include	<zoffparts.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |NYEARS_CENTURY| */

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

char *timestr_std(time_t t,char *buf) noex {
	return timestr_date(t,buf,timestrtype_std) ;
} /* end subroutine (timestr_std) */

char *timestr_edate(time_t t,char *buf) noex {
	return timestr_date(t,buf,timestrtype_std) ;
} /* end subroutine (timestr_edate) */

char *timestr_gmtstd(time_t t,char *buf) noex {
	return timestr_date(t,buf,timestrtype_gmstd) ;
} /* end subroutine (timestr_gmtstd) */

char *timestr_msg(time_t t,char *buf) noex {
	return timestr_date(t,buf,timestrtype_msg) ;
} /* end subroutine (timestr_msg) */

char *timestr_hdate(time_t t,char *buf) noex {
	return timestr_date(t,buf,timestrtype_msg) ;
} /* end subroutine (timestr_hdate) */

char *timestr_log(time_t t,char *buf) noex {
	return timestr_date(t,buf,timestrtype_log) ;
} /* end subroutine (timestr_log) */

#ifdef	COMMENT
char *timestr_loggm(time_t t,char *buf) noex {
	return timestr_date(t,buf,timestrtype_gmlog) ;
} /* end subroutine (timestr_loggm) */
#endif /* COMMENT */

char *timestr_gmlog(time_t t,char *buf) noex {
	return timestr_date(t,buf,timestrtype_gmlog) ;
} /* end subroutine (timestr_gmlog) */

char *timestr_logz(time_t t,char *buf) noex {
	return timestr_date(t,buf,timestrtype_logz) ;
} /* end subroutine (timestr_logz) */

#ifdef	COMMENT
char *timestr_loggmz(time_t t,char *buf) noex {
	return timestr_date(t,buf,timestrtype_gmlogz) ;
} /* end subroutine (timestr_loggmz) */
#endif /* COMMENT */

char *timestr_gmlogz(time_t t,char *buf) noex {
	return timestr_date(t,buf,timestrtype_gmlogz) ;
} /* end subroutine (timestr_gmlogz) */

/* create a date-string as specified by its type-code */
char *timestr_date(time_t t,char *tbuf,int type) noex {
	cint		tlen = TIMEBUFLEN ;
	int		rs = SR_FAULT ;
	if (tbuf) ylikely {
	    tbuf[0] = '\0' ;
	    rs = SR_DOM ;
	    if (t >= 0) ylikely {
	        tmtime	tmt ;
	        bool	f_gmt = false ;
	        switch (type) {
	        case timestrtype_gmstd:
	        case timestrtype_gmlog:
	        case timestrtype_gmlogz:
	            f_gmt = true ;
	            break ;
	        } /* end switch */
		/* split the time into its component parts */
	        if (f_gmt) {
	            rs = tmtime_timegm(&tmt,t) ;
	        } else {
	            rs = tmtime_timelocal(&tmt,t) ;
	        }
        	/* create the appropriate string based on the type-code */
	        if (rs >= 0) ylikely {
		    cchar	*fmt = nullptr ;
	            switch (type) {
	            case timestrtype_std:
	            case timestrtype_gmstd:
	                fmt = "%a %b %d %T %Z %Y %O" ;
	                break ;
	            case timestrtype_msg:
			fmt = "%d %b %Y %T %O (%Z)" ;
	                break ;
	            case timestrtype_log:
	            case timestrtype_gmlog:
			fmt = "%y%m%d_%H%M:%S" ;
	                break ;
	            case timestrtype_logz:
	            case timestrtype_gmlogz:
			fmt = "%y%m%d_%H%M:%S_%Z" ;
	                break ;
	            default:
	                rs = sncpy1(tbuf,tlen,"** invalid type **") ;
	                break ;
	            } /* end switch */
		    if (fmt) ylikely {
	                rs = sntmtime(tbuf,tlen,&tmt,fmt) ;
		    }
	        } /* end if (ok) */
	    } /* end if (valid) */
	    if (rs < 0) {
		tbuf[0] = '\0' ;
	    } /* end if (error) */
	} /* end if (non-null) */
	if (rs < 0) {
	    ulogerror("timestr",rs,"date") ;
	} /* end if (error) */
	return (rs >= 0) ? tbuf : nullptr ;
} /* end subroutine (timestr_date) */



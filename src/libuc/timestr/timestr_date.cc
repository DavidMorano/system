/* timestr_date SUPPORT */
/* lang=C++20 */

/* convert UNIX® time into a various date formats */
/* version %I% last-modified %G% */

#define	CF_SNTMTIME	1		/* use |sntmtime(3dam)| */

/* revision history:

	= 1998-08-01, David A­D­ Morano
	This subroutine was originally written.

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
 
	Return the date (in UNIX® mail evelope format) into the
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
#include	<tzfile.h>		/* for TM_YEAR_BASE */
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<usupport.h>		/* |ulogerror(3u)| */
#include	<snx.h>
#include	<sncpyx.h>
#include	<tmtime.h>
#include	<zoffparts.h>
#include	<localmisc.h>		/* |TIMEBUFLEN| + |NYEARS_CENTURY| */

#include	"timestr.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

extern "C" {
    char *timestr_date(time_t,int,char *) noex ;
}


/* local variables */

#if	CF_SNTMTIME

#else /* CF_SNTMTIME */

static cchar	*months[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
} ;

static cchar	*days[] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
} ;

#endif /* CF_SNTMTIME */


/* exported variables */


/* exported subroutines */

char *timestr_std(time_t t,char *buf) noex {
	return timestr_date(t,timestrtype_std,buf) ;
}
/* end subroutine (timestr_std) */

char *timestr_edate(time_t t,char *buf) noex {
	return timestr_date(t,timestrtype_std,buf) ;
}
/* end subroutine (timestr_edate) */

char *timestr_gmtstd(time_t t,char *buf) noex {
	return timestr_date(t,timestrtype_gmstd,buf) ;
}
/* end subroutine (timestr_gmtstd) */

char *timestr_msg(time_t t,char *buf) noex {
	return timestr_date(t,timestrtype_msg,buf) ;
}
/* end subroutine (timestr_msg) */

char *timestr_hdate(time_t t,char *buf) noex {
	return timestr_date(t,timestrtype_msg,buf) ;
}
/* end subroutine (timestr_hdate) */

char *timestr_log(time_t t,char *buf) noex {
	return timestr_date(t,timestrtype_log,buf) ;
}
/* end subroutine (timestr_log) */

#ifdef	COMMENT
char *timestr_loggm(time-t t,char *buf) noex {
	return timestr_date(t,timestrtype_gmlog,buf) ;
}
/* end subroutine (timestr_loggm) */
#endif /* COMMENT */

char *timestr_gmlog(time_t t,char *buf) noex {
	return timestr_date(t,timestrtype_gmlog,buf) ;
}
/* end subroutine (timestr_gmlog) */

char *timestr_logz(time_t t,char *buf) noex {
	return timestr_date(t,timestrtype_logz,buf) ;
}
/* end subroutine (timestr_logz) */

#ifdef	COMMENT
char *timestr_loggmz(time_t t,char *buf) noex {
	return timestr_date(t,timestrtype_gmlogz,buf) ;
}
/* end subroutine (timestr_loggmz) */
#endif /* COMMENT */

char *timestr_gmlogz(time_t t,char *buf) noex {
	return timestr_date(t,timestrtype_gmlogz,buf) ;
}
/* end subroutine (timestr_gmlogz) */

/* create a date-string as specified by its type-code */
char *timestr_date(time_t t,int type,char *tbuf) noex {
	cint		tlen = TIMEBUFLEN ;
	int		rs = SR_FAULT ;
	if (tbuf) {
	    tbuf[0] = '\0' ;
	    rs = SR_DOM ;
	    if (t >= 0) {
	TMTIME		tmt ;
	int		f_gmt = false ;
	switch (type) {
	case timestrtype_gmstd:
	case timestrtype_gmlog:
	case timestrtype_gmlogz:
	    f_gmt = true ;
	    break ;
	} /* end switch */

/* split the time into its component parts */

	if (f_gmt) {
	    rs = tmtime_gmtime(&tmt,t) ;
	} else {
	    rs = tmtime_localtime(&tmt,t) ;
	}

/* create the appropriate string based on the type-code */

	if (rs >= 0) {
	    switch (type) {

	    case timestrtype_std:
	    case timestrtype_gmstd:

#if	CF_SNTMTIME
	        rs = sntmtime(tbuf,tlen,&tmt,"%a %b %d %T %Z %Y %O") ;
#else
	        {
	            ZOFFPARTS	zo ;
	            zoffparts_set(&zo,tmt.gmtoff) ;
	            rs = snwprintf(tbuf,tlen,
	                "%t %t %2u %02u:%02u:%02u %s %04u %c%02u%02u",
	                days[tmt.wday],3,
	                months[tmt.mon],3,
	                tmt.mday,
	                tmt.hour,
	                tmt.min,
	                tmt.sec,
	                tmt.zname,
	                (tmt.year + TM_YEAR_BASE),
	                ((tmt.gmtoff >= 0) ? '-' : '+'),
	                zo.hours,zo.mins) ;
	        }
#endif /* CF_SNTMTIME */

	        break ;

	    case timestrtype_msg:

#if	CF_SNTMTIME
	        rs = sntmtime(tbuf,tlen,&tmt,"%d %b %Y %T %O (%Z)") ;
#else
	        {
	            ZOFFPARTS	zo ;
	            zoffparts_set(&zo,tmt.gmtoff) ;
	            rs = snwprintf(tbuf,tlen,
	                "%2u %t %4u %02u:%02u:%02u %c%02u%02u (%s)",
	                tmt.mday,
	                months[tmt.mon],3,
	                (tmt.year + TM_YEAR_BASE),
	                tmt.hour,
	                tmt.min,
	                tmt.sec,
	                ((tmt.gmtoff >= 0) ? '-' : '+'),
	                zo.hours,zo.mins,
	                tmt.zname) ;
	        }
#endif /* CF_SNTMTIME */

	        break ;

	    case timestrtype_log:
	    case timestrtype_gmlog:

#if	CF_SNTMTIME
	        rs = sntmtime(tbuf,tlen,&tmt,"%y%m%d_%H%M:%S") ;
#else
	        rs = snwprintf(tbuf,tlen,
	            "%02u%02u%02u_%02u%02u:%02u",
	            (tmt.year % NYEARS_CENTURY),
	            (tmt.mon + 1),
	            tmt.mday,
	            tmt.hour,
	            tmt.min,
	            tmt.sec) ;
#endif /* CF_SNTMTIME */

	        break ;

	    case timestrtype_logz:
	    case timestrtype_gmlogz:

#if	CF_SNTMTIME
	        rs = sntmtime(tbuf,tlen,&tmt,"%y%m%d_%H%M:%S_%Z") ;
#else
	        rs = snwprintf(tbuf,tlen,
	            "%02u%02u%02u_%02u%02u:%02u_%s",
	            (tmt.year % NYEARS_CENTURY),
	            (tmt.mon + 1),
	            tmt.mday,
	            tmt.hour,
	            tmt.min,
	            tmt.sec,
	            tmt.zname) ;
#endif /* CF_SNTMTIME */

	        break ;

	    default:
	        rs = sncpy1(tbuf,tlen,"** invalid type **") ;
	        break ;
	    } /* end switch */
	} /* end if (ok) */
	    } /* end if (valid) */
	    if (rs < 0) tbuf[0] = '\0' ;
	} /* end if (non-null) */
	if (rs < 0) {
	    ulogerror("timestr",rs,"date") ;
	}
	return (rs >= 0) ? tbuf : nullptr ;
}
/* end subroutine (timestr_date) */



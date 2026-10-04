/* strtime_date SUPPORT */
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
	strtime_date
 
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

	Synopsis:
	char *strtime_date(time_t t,char *tbuf,strtimetypes type) noex

	Arguments:
	t		time to convert (format)
	rbuf		result buffer pointer
	type		type of convesion (format) to perform

	Returns:
	non-NULL	pointer to given result buffer
	NULL		conversion failed

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

#include	"strtime.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

using tst_tx	= strtimetypes ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	snrender	(char *,int,time_t,strtimetypes,cc *) noex ;
local bool	typelocal	(strtimetypes) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

char *strtime_std(time_t t,char *buf) noex {
	return strtime_date(t,buf,strtimetype_std) ;
} /* end subroutine (strtime_std) */

char *strtime_edate(time_t t,char *buf) noex {
	return strtime_date(t,buf,strtimetype_std) ;
} /* end subroutine (strtime_edate) */

char *strtime_gmtstd(time_t t,char *buf) noex {
	return strtime_date(t,buf,strtimetype_gmstd) ;
} /* end subroutine (strtime_gmtstd) */

char *strtime_msg(time_t t,char *buf) noex {
	return strtime_date(t,buf,strtimetype_msg) ;
} /* end subroutine (strtime_msg) */

char *strtime_hdate(time_t t,char *buf) noex {
	return strtime_date(t,buf,strtimetype_msg) ;
} /* end subroutine (strtime_hdate) */

char *strtime_log(time_t t,char *buf) noex {
	return strtime_date(t,buf,strtimetype_log) ;
} /* end subroutine (strtime_log) */

#ifdef	COMMENT
char *strtime_loggm(time_t t,char *buf) noex {
	return strtime_date(t,buf,strtimetype_gmlog) ;
} /* end subroutine (strtime_loggm) */
#endif /* COMMENT */

char *strtime_gmlog(time_t t,char *buf) noex {
	return strtime_date(t,buf,strtimetype_gmlog) ;
} /* end subroutine (strtime_gmlog) */

char *strtime_logz(time_t t,char *buf) noex {
	return strtime_date(t,buf,strtimetype_logz) ;
} /* end subroutine (strtime_logz) */

#ifdef	COMMENT
char *strtime_loggmz(time_t t,char *buf) noex {
	return strtime_date(t,buf,strtimetype_gmlogz) ;
} /* end subroutine (strtime_loggmz) */
#endif /* COMMENT */

char *strtime_gmlogz(time_t t,char *buf) noex {
	return strtime_date(t,buf,strtimetype_gmlogz) ;
} /* end subroutine (strtime_gmlogz) */

/* create a date-string as specified by its type-code */
char *strtime_date(time_t t,char *tbuf,strtimetypes type) noex {
	cint		tlen = TIMEBUFLEN ;
	int		rs = SR_FAULT ;
	if (tbuf) ylikely {
	    tbuf[0] = '\0' ;
	    rs = SR_DOM ;
	    if (t >= 0) ylikely {
		cchar	*fmt = nullptr ;
		rs = SR_OK ;
	        switch (type) {
	        case strtimetype_std:
	        case strtimetype_gmstd:
	            fmt = "%a %b %d %T %Z %Y %O" ;
	            break ;
	        case strtimetype_msg:
		    fmt = "%d %b %Y %T %O (%Z)" ;
	            break ;
	        case strtimetype_log:
	        case strtimetype_gmlog:
		    fmt = "%y%m%d_%H%M:%S" ;
	            break ;
	        case strtimetype_logz:
	        case strtimetype_gmlogz:
		    fmt = "%y%m%d_%H%M:%S_%Z" ;
	            break ;
	        default:
	            rs = sncpy1(tbuf,tlen,"** invalid type **") ;
	            break ;
	        } /* end switch */
		if ((rs >= 0) && fmt) ylikely {
		    rs = snrender(tbuf,tlen,t,type,fmt) ;
		} /* end if (ok) */
	        if (rs < 0) {
		    tbuf[0] = '\0' ;
	        } /* end if (error) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	if (rs < 0) {
	    ulogerror("strtime",rs,"date") ;
	} /* end if (error) */
	return (rs >= 0) ? tbuf : nullptr ;
} /* end subroutine (strtime_date) */


/* local subroutines */

local int snrender(char *tbuf,int tlen,time_t t,tst_tx type,cc *fmt) noex {
	int		rs ;
	cbool		flocal = typelocal(type) ;
	if (tmtime tmt ; (rs = tmt.timex(t,flocal)) >= 0) ylikely {
	    rs = sntmtime(tbuf,tlen,&tmt,fmt) ;
	} /* end if (tmtime) */
	return rs ;
} /* end subroutine (snrender) */

local bool typelocal(strtimetypes type) noex {
    	bool	flocal = true ;
	switch (type) {
	case strtimetype_gmstd:
	case strtimetype_gmlog:
	case strtimetype_gmlogz:
	    flocal = false ;
	    break ;
	default:
	    break ;
	} /* end switch */
	return flocal ;
} /* end subroutine (typelocal) */



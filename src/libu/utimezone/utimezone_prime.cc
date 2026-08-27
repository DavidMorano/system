/* utimezone_prime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* time-zone information retrieval */
/* version %I% last-modified %G% */

#define	CF_OPTMIZE	0		/* try to optimize retrieval */

/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/*******************************************************************************

  	Names:
	u_timezone
	u_tzname

	Description:
	These subroutines retrieve information about the current
	time-zone for the current process.  This retrieved information
	is constant over the lifetime of the process.

	Synopsis:
	u_timezone(TIMEZONE *tzp) noex
	u_tzname(char *rbuf,int rlen) noex

	Arguments:
	tzp		timezone object pointer
	rbuf		result buffer pointer
	rlen		result buffer length

	Returns:
	>=0		result buffer returned number of bytes
	<0		error (system-return) 

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/time.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD placement-new */
#include	<string>		/* C++STD */
#include	<chrono>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |sncpy(3u)| */
#include	<usys.h>		/* LIBU */
#include	<umem.hh>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"utimezone.h"

import utimezones ;

/* local defines */

#ifndef	CF_OPTMIZE
#define	CF_OPTMIZE	1		/* try to optimize retrieval */
#endif


/* local namespaces */

using std::string ;			/* type */
using libu::sncpy ;			/* subroutine */
using libu::snwcpy ;			/* subroutine */
using libu::uftime ;			/* subroutine */
using libu::ugettimeofday ;		/* subroutine */
using libu::umem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct retriever {
	TIMEZONE	tz ;
	charp		tznp ;
	int		tznl ;
	operator int 	() noex ;
	int divine	() noex ;
	int tzload	(cchar *,int = -1) noex ;
	void dtor	() noex ;
	destruct retriever() {
	    if (tznp) dtor() ;
	} ; /* end destruct */
    } ; /* end struct (retriever) */
    struct initer {
	bool finit ;
	operator int () noex ;
    } ; /* emd struct (initer) */
} /* end namespace */


/* forward references */


/* local variables */

static initer		init ;
static retriever	tzdata ;
cbool			f_optimize	= CF_OPTMIZE ;


/* exported variables */


/* exported subroutines */

int u_timezone(TIMEZONE *tzp) noex {
    	int		rs = SR_FAULT ;
	if (tzp) ylikely {
	    if (cint rsi = init ; (rs = rsi) >= 0) {
		*tzp = tzdata.tz ; /* copy */
	    } /* end if (initer) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (u_timezone) */

int u_tzname(char *rbuf,int rlen) noex {
    	int		rs = SR_FAULT ;
	if (rbuf) ylikely {
	    rs = SR_INVALID ;
	    if (rlen > 0) {
	       if (cint rsi = init ; (rs = rsi) >= 0) {
		   rs = SR_BUGCHECK ;
		   if (tzdata.tznp) {
		       rs = sncpy(rbuf,rlen,tzdata.tznp) ;
		   }
	       } /* end if (initer) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (u_tzname) */


/* local subroutines */

retriever::operator int () noex {
    	int		rs ;
	if (syshas.timezone && f_optimize) {
	    TIMEZONE td ;
	    if (timeval tv ; (rs = ugettimeofday(&tv,&td)) >= 0) {
		tz.tz_minuteswest	= td.tz_minuteswest ;
		tz.tz_dsttime		= td.tz_dsttime ;
	    } else {
		tz.tz_dsttime = -1 ;
	    } /* end if (uftime) */
	} else {
	    rs = divine() ;
	} /* end if */
	return rs ;
} /* end method (retriever::operator) */

int retriever::divine() noex {
    	int		rs = SR_OK ;
        try {
	    cauto tzp = std::chrono::current_zone() ; // may throw
            cauto now = std::chrono::system_clock::now() ;
            const std::chrono::zoned_time cur_time(tzp,now) ;
	    {
	        using namespace std::chrono ;
	        cauto info = tzp->get_info(now) ;
	        {
		    con string  &abbr  = info.abbrev ;
		    seconds s_off = seconds(info.offset) ;
		    seconds s_sav = seconds(info.save) ;
		    {
			ccharp ap = abbr.c_str() ;
			cauto abbrlen = abbr.length() ;
		        cauto roff = s_off.count() ;
		        cauto rsav = s_sav.count() ;
			{
			    cint   al = conv<int>(abbrlen) ;
			    int woff = (neg conv<int>(roff / 60)) ;
			    int wsav = (conv<int>(rsav) / 60) ;
			    tz.tz_minuteswest	= (woff + wsav) ;
			    tz.tz_dsttime	= (rsav != 0)  ;
			    rs = tzload(ap,al) ;
			} /* end if (block) */
		    } /* end block */
		} /* end block */
	    } /* end block */
        } catch (const std::runtime_error&) {
	    rs = SR_IO ;
	    tz.tz_dsttime = -1 ;
	} catch (...) {
	    tz.tz_dsttime = -1 ;
	    rs = SR_BUGCHECK ;
        } /* end if (catch) */
        return rs ;
} /* end merhod (retriever::divine) */

int retriever::tzload(cchar *ap,int al) noex {
    	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (ap) {
	    rs = SR_INVALID ;
	    if (al > 0) {
	        rs = SR_NOMEM ;
	        if (tznp = new(nt) char [al + 1] ; tznp) {
	            tznl = al ;
	            rs = snwcpy(tznp,tznl,ap,al) ;
	        } /* end if (new-char) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end merhod (retriever::tzname) */

void retriever::dtor() noex {
	if (tznp) {
	    delete [] tznp ;
	    tznp = nullptr ;
	    tznl = 0 ;
	} /* end if (delete-char) */
} /* end merhod (retriever::dtor) */

initer::operator int () noex {
    	int		rs = SR_OK ;
	if (! finit) {
	    if (static cint rst = tzdata ; (rs = rst) >= 0) {
	        finit = true ;
	    } /* end if (retriever) */
	} /* end if (needed) */
	return rs ;
} /* end method (initier::operator) */



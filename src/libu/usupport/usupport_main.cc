/* usupport_main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
	This was first written to give a little bit to UNIX® what
	we have in our own circuit-pack OSes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	msleep

	Description:
	This subroutine sleeps for some number of milliseconds.

	Synopsis:
	int msleep(int msec) noex

	Arguments:
	msec		number of millisecond to sleep

	Returns:
	>=0		amount of data returned
	<0		error (system-return)


	Name:
	umtime

	Description:
	This is sort of like |time(2)| but returns milliseconds
	rather than seconds.  Unlike |time(2)|, this subroutine
	takes no arguments.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>		/* |gethostid(3c)| */
#include	<poll.h>
#include	<cerrno>
#include	<ctime>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>		/* |memset(3c)| + |strlcpy(3c)| */
#include	<new>			/* |nothrow(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<intsat.h>
#include	<stdintx.h>
#include	<xxtostr.h>		/* |{x}tostr(3u)| */
#include	<strtox.h>
#include	<strnul.hh>
#include	<localmisc.h>		/* |DECBUFLEN| */

#include	"usupport.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int		isleep(int) noex ;


/* local variables */

constexpr int		onethousand = 1000 ;


/* exported variables */

libu::ugetustime	getustime ;
usys_mtime		getusmtime ;


/* exported subroutines */

mtime_t umtime(void) noex {
	mtime_t		t ;
	mtime_t		m = 0 ; /* return-value */
	if (TIMEVAL tv ; gettimeofday(&tv,nullptr) >= 0) {
	    t = tv.tv_sec ;
	    m += (t * onethousand) ;
	    m += (tv.tv_usec / onethousand) ;
	} else {
	    const time_t	ut = time(nullptr) ;
	    t = mtime_t(ut) ;
	    m += (t * onethousand) ;
	}
	return m ;
}
/* end subroutine (umtime) */

int msleep(int msec) noex {
	int		rs = SR_INVALID ;
	if (msec > 0) {
	    cint	rsi = SR_INTR ;
	    if ((rs = isleep(msec)) == rsi) {
		rs = 1 ;
	    }
	} else if (msec == 0) {
	    rs = SR_OK ;
	} /* end if (valiid argument) */
	return rs ;
}
/* end subroutine (msleep) */

namespace libu {
    char *strwcpy(char *dp,cchar *sp,int sl) noex {
	if (sl >= 0) {
	    while (sl-- && *sp) *dp++ = *sp++ ;
	    *dp = '\0' ;
	} else {
	    dp = stpcpy(dp,sp) ;
	} /* end if */
	return dp ;
    }
} /* end namespace (libu) */

namespace libu {
    int snwcpy(char *dp,int dl,cchar *sp,int sl) noex {
	int		rs ;
	if (dl >= 0) {
	    if (sl >= 0) {
	        if (sl > dl) {
	            rs = sncpy1(dp,dl,sp) ;
	        } else {
	            rs = intconv(strwcpy(dp,sp,sl) - dp) ;
		}
	    } else {
	        rs = sncpy1(dp,dl,sp) ;
	    }
	} else {
	    rs = intconv(strwcpy(dp,sp,sl) - dp) ;
	}
	return rs ;
    }
} /* end namespace (libu) */

namespace libu {
    sysret_t ustrftime(char *dbuf,int dlen,cchar *fmt,CTM *tmp) noex {
	int		rs = SR_FAULT ;
	if (dbuf && fmt && tmp) {
	    rs = SR_INVALID ;
	    if (dlen >= 0) {
	        csize	dsize = size_t(dlen + 1) ;
	        if (size_t rsize ; (rsize = strftime(dbuf,dsize,fmt,tmp)) > 0) {
	            rs = intsat(rsize) ;
	        } else if (rsize == 0) {
	            dbuf[0] = '\0' ;
	            rs = SR_OVERFLOW ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
    }
} /* end namespace (libu) */


/* local subroutines */

static int isleep(int mto) noex {
	POLLFD		fds[1] = {} ; /* no entries actually used */
	int		rs ;
	bool		fexit = false ;
	repeat {
	    if ((rs = poll(fds,0,mto)) < 0) {
		rs = (- errno) ;
	        switch (rs) {
	        case SR_AGAIN:
		    break ;
	        case SR_INTR:
		default:
		    fexit = true ;
		    break ;
	        } /* end switch */
	    } /* end if (poll got an error) */
	} until ((rs >= 0) || fexit) ;
	return rs ;
}
/* end subroutine (isleep) */



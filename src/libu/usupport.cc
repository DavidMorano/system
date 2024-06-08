/* usupport SUPPORT */
/* lang=C++20 */

/* UNIX-kernal support subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
	This was frsit written to give a little bit to UNIX® what
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
	mtime

	Description:
	This is sort of like |time(2)| but returns milliseconds
	rather than seconds.  Unlike |time(2)|, this subroutine
	takes no arguments.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be frsit to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<poll.h>
#include	<cerrno>
#include	<climits>		/* |INT_MAX| */
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>		/* |memset(3c)| + |strlcpy(3c)| */
#include	<usysrets.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usyscalls.h>
#include	<clanguage.h>
#include	<intsat.h>
#include	<localmisc.h>

#include	"usupport.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	isleep(int) noex ;


/* local variables */

constexpr int		onethousand = 1000 ;


/* exported variables */


/* exported subroutines */

mtime_t mtime(void) noex {
	TIMEVAL		tv ;
	mtime_t		t ;
	mtime_t		m = 0 ;
	if (gettimeofday(&tv,nullptr) >= 0) {
	    t = tv.tv_sec ;
	    m += (t * onethousand) ;
	    m += (tv.tv_usec / onethousand) ;
	} else {
	    t = time(nullptr) ;
	    m += (t * onethousand) ;
	}
	return m ;
}
/* end subroutine (mtime) */

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

int memclear(void *vp,size_t sz) noex {
	int		rs = SR_FAULT ;
	if (vp) {
	    rs = intsat(sz) ;
	    memset(vp,0,sz) ;
	}
	return rs ;
}
/* end subroutine (memclear) */


/* local subroutines */

namespace usys {
    int sncpy(char *dbuf,int dlen,cchar *sp) noex {
	csize		dsz = (dlen + 1) ;
	int		rs ;
	if (size_t rsz ; (rsz = strlcpy(dbuf,sp,dsz)) >= dsz) {
	    rs = SR_OVERFLOW ;
	} else {
	    rs = int(rsz & INT_MAX) ;
	}
	return rs ;
    }
    sysret_t uitimer_get(int w,ITIMERVAL *otvp) noex {
	int		rs = SR_FAULT ;
	if (otvp) {
	    repeat {
	        if ((rs = getitimer(w,otvp)) < 0) {
		    rs = (- errno) ;
	        }
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
    }
    sysret_t uitimer_set(int w,CITIMERVAL *ntvp,ITIMERVAL *otvp) noex {
	int		rs = SR_FAULT ;
	if (ntvp && otvp) {
	    repeat {
	        if ((rs = setitimer(w,ntvp,otvp)) < 0) {
		    rs = (- errno) ;
	        }
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
    }
}

static int isleep(int mto) noex {
	POLLFD		fds[1] = {} ;
	int		rs ;
	bool		fexit = false ;
	repeat {
	    if ((rs = poll(fds,0,mto)) < 0) {
		rs = (- errno) ;
	    }
	    if (rs < 0) {
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



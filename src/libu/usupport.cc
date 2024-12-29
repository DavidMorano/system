/* usupport SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* UNIX® kernel support subroutines */
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
	umtime

	Description:
	This is sort of like |time(2)| but returns milliseconds
	rather than seconds.  Unlike |time(2)|, this subroutine
	takes no arguments.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be frsit to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>		/* |gethostid(3c)| */
#include	<poll.h>
#include	<cerrno>
#include	<climits>		/* |INT_MAX| */
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>		/* |memset(3c)| + |strlcpy(3c)| */
#include	<new>
#include	<usysrets.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usyscalls.h>
#include	<clanguage.h>
#include	<intsat.h>
#include	<stdintx.h>
#include	<xxtostr.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"usupport.h"


/* local defines */


/* imported namespaces */

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
usys_mtime		getmtime ;
usys_mtime		mtime ;


/* exported subroutines */

mtime_t umtime(void) noex {
	mtime_t		t ;
	mtime_t		m = 0 ;
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

int memclear(void *vp,size_t sz) noex {
	int		rs = SR_FAULT ;
	if (vp) {
	    rs = intsat(sz) ;
	    memset(vp,0,sz) ;
	}
	return rs ;
}
/* end subroutine (memclear) */

namespace libu {
    char *strwcpy(char *dp,cchar *sp,int sl) noex {
	if (sl >= 0) {
	    while (sl-- && *sp) *dp++ = *sp++ ;
	} else {
	    while (*sp) *dp++ = *sp++ ;
	} /* end if */
	*dp = '\0' ;
	return dp ;
    }
}

namespace libu {
    int sncpy1(char *dbuf,int dlen,cchar *sp) noex {
	int		rs = SR_FAULT ;
	if (dbuf) {
	    rs = SR_INVALID ;
	    if (dlen >= 0) {
	        csize	dsz = (dlen + 1) ;
	        if (size_t rsz ; (rsz = strlcpy(dbuf,sp,dsz)) >= dsz) {
	            rs = SR_OVERFLOW ;
	        } else {
	            rs = intsat(rsz) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
    }
    int snwcpy(char *dp,int dl,cchar *sp,int sl) noex {
	int		rs ;
	if (dl >= 0) {
	    if (sl >= 0) {
	        if (sl > dl) {
	            rs = sncpy1(dp,dl,sp) ;
	        } else {
	            rs = strwcpy(dp,sp,sl) - dp ;
		}
	    } else {
	        rs = sncpy1(dp,dl,sp) ;
	    }
	} else {
	    rs = strwcpy(dp,sp,sl) - dp ;
	}
	return rs ;
    }
} /* end namespace (libu) */

namespace libu {
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
    sysret_t ustrftime(char *dbuf,int dlen,cchar *fmt,CTM *tmp) noex {
	int		rs = SR_FAULT ;
	if (dbuf && fmt && tmp) {
	    rs = SR_INVALID ;
	    if (dlen >= 0) {
	        csize	dsz = size_t(dlen + 1) ;
	        if (size_t rsz ; (rsz = strftime(dbuf,dsz,fmt,tmp)) > 0) {
	            rs = intsat(rsz) ;
	        } else if (rsz == 0) {
	            dbuf[0] = '\0' ;
	            rs = SR_OVERFLOW ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
    }
} /* end namespace (libu) */

namespace libu {
    template<typename T>
    static int ctdecx(charp (*ctx)(T,char *),char *dp,int dl,T uv) noex {
	int		rs = SR_FAULT ;
	if (dp) {
	    cint	dlen = DIGBUFLEN ;
	    char	dbuf[DIGBUFLEN + 1] ;
	    char	*bp = ctx(uv,(dbuf+dlen)) ;
	    rs = sncpy(dp,dl,bp) ;
	}
	return rs ;
    }
    int ctdecui(char *dp,int dl,uint uv) noex {
	return ctdecx(uitostr,dp,dl,uv) ;
    }
    int ctdecul(char *dp,int dl,ulong uv) noex {
	return ctdecx(ultostr,dp,dl,uv) ;
    }
    int ctdecull(char *dp,int dl,ulonglong uv) noex {
	return ctdecx(ulltostr,dp,dl,uv) ;
    }
} /* end namespace (libu) */


/* local subroutines */

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



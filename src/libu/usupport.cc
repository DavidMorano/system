/* usupport SUPPORT */
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
	mtime

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

static int	isleep(int) noex ;


/* local variables */

constexpr int	onethousand = 1000 ;


/* exported variables */

ugetustime	getustime ;


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
	csize		dsz = (dlen + 1) ;
	int		rs ;
	if (size_t rsz ; (rsz = strlcpy(dbuf,sp,dsz)) >= dsz) {
	    rs = SR_OVERFLOW ;
	} else {
	    rs = int(rsz & INT_MAX) ;
	}
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

namespace libu {
    sysret_t loadhostid(char *dp,int dl) noex {
	int		rs = SR_FAULT ;
	if (dp) {
	    if (ulong hid ; (rs = ugethostid(&hid)) >= 0) {	
		rs = ctdec(dp,dl,hid) ;
	    }
	}
	return rs ;
    }
    sysret_t ugethostid(ulong *idp) noex {
	int		rs = SR_FAULT ;
	if (idp) {
	    clong	res = gethostid() ;
	    *idp = ulong(res) ;
	}
	return rs ;
    }
}

namespace libu {
    template<typename T>
    int ctdecx(charp (*ctx)(T,char *),char *dp,int dl,T uv) noex {
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
}


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



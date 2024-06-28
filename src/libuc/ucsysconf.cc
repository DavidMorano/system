/* ucsysconf SUPPORT */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* get system configuration information */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_sysconfval

	Description:
	This subroutine returns system-related values (as opposed
	to strings as |uc_sysconfstr(3uc)| does).

	Synopsis:
	int uc_sysconfval(int req,long *rp) noex

	Arguments:
	req		requested value to return
	rp		pointer to value-type |long| for received value


	Name:
	uc_sysconfstr

	Description:
	This subroutine is NOT the same as the UNIX®-System version.
	This subroutine returns "OVERFLOW" when the user supplied
	buffer is not big enough to hold the associated value.

	Synopsis:
	int uc_sysconfstr(char *rbuf,int rlen,int req) noex

	Arguments:
	rbuf		user supplied buffer to hold result
	rlen		length of user supplied value
	req		configuration value to request

	Returns:
	>0		valid and the value is returned with this length
	0		valid but there was no value associated
	<0		the requested configuration value was invalid

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<cerrno>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<atomic>
#include	<usystem.h>
#include	<errtimer.hh>
#include	<localmisc.h>

#include	"ucsysconf.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::atomic_int ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

enum dataitems {
	dataitem_maxarg,
	dataitem_maxline,
	dataitem_maxlogin,
	dataitem_maxhost,
	dataitem_overlast
} ;

namespace {
    struct ucconfdatas {
	atomic_int	d[dataitem_overlast] ;
    } ; /* end struct (ucconfdatas) */
    struct ucsysconf ;
    typedef int (ucsysconf::*usysconf_m)(int) ;
    struct ucsysconf {
	usysconf_m	m ;
	char		*rbuf ;
	long		*lp ;
	int		rlen ;
	ucsysconf(char *rp,int rl) noex : rbuf(rp), rlen(rl) { } ;
	ucsysconf(long *p) noex : lp(p) { } ;
	int mconfval(int) noex ;
	int mconfstr(int) noex ;
	int operator () (int) noex ;
	int cache(int) noex ;
    } ; /* end struct (ucsysconf) */
}


/* forward references */


/* local variables */

static ucconfdatas	ucdata ;


/* exported variables */


/* exported subroutines */

int uc_sysconfval(int req,long *rp) noex {
	ucsysconf	sco(rp) ;
	int		rs = SR_OK ;
	sco.m = &ucsysconf::mconfval ;
	switch (req) {
	case _SC_ARG_MAX:
	case _SC_LINE_MAX:
	case _SC_HOST_NAME_MAX:
	case _SC_LOGIN_NAME_MAX:
	    rs = sco.cache(req) ;
	    break ;
	default:
	    rs = sco(req) ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end subroutine (uc_sysconfval) */

int uc_sysconfstr(char *rbuf,int rlen,int req) noex {
	ucsysconf	sco(rbuf,rlen) ;
	sco.m = &ucsysconf::mconfstr ;
	return sco(req) ;
}
/* end subroutine (uc_sysconfstr) */


/* local subroutines */

int ucsysconf::operator () (int req) noex {
	errtimer	to_again	= utimeout[uto_again] ;
	errtimer	to_busy		= utimeout[uto_busy] ;
	errtimer	to_nomem	= utimeout[uto_nomem] ;
	reterr		r ;
	int		rs ;
	repeat {
	    if ((rs = (this->*m)(req)) < 0) {
		r(rs) ;			/* <- default causes exit */
                switch (rs) {
                case SR_AGAIN:
                    r = to_again(rs) ;
                    break ;
                case SR_BUSY:
                    r = to_busy(rs) ;
                    break ;
                case SR_NOMEM:
                    r = to_nomem(rs) ;
                    break ;
	        case SR_INTR:
		    r(false) ;
	            break ;
	        } /* end switch */
		rs = r ;
	    } /* end if (std-call) */
	} until ((rs >= 0) || r.fexit) ;
	return rs ;
}
/* end subroutine (ucsysconf::operator) */

int ucsysconf::mconfval(int req) noex {
	int		rs = SR_OK ;
	errno = 0 ;
	if (long res ; (res = sysconf(req)) >= 0L) {
	    if (lp) *lp = res ;
	    rs = intsat(res) ;
	} else {
	    rs = (errno) ? (- errno) : SR_NOTSUP ;
	}
	return rs ;
}
/* end subroutine (ucsysconf::mconfval) */

int ucsysconf::mconfstr(int req) noex {
	size_t		res ;
	int		rs = SR_OK ;
	int		len = 0 ;
	errno = 0 ;
	if (rlen > 0) {
	    rs = SR_FAULT ;
	    if (rbuf) {
	        csize	rsz = (rlen+1) ;
		rs = SR_OK ;
	        if ((res = confstr(req,rbuf,rsz)) == 0uz) {
	            rs = (errno) ? (- errno) : SR_NOTSUP ;
	        } else if (res > 0) {
	            if (res <= rsz) {
	                len = intsat(res-1) ;
	            } else {
		        rs = SR_OVERFLOW ;
		    }
		} else {
		    rs = SR_NOSYS ;	/* not defined in documentation */
	        } /* end if */
	    } /* end if (non-null) */
	} else if (rlen == 0) {
	    cnullptr	np{} ;
	    if ((res = confstr(req,np,0uz)) == 0uz) {
	        rs = (errno) ? (- errno) : SR_NOTSUP ;
	    } else if (res > 0) {
	        len = intsat(res-1) ;
	    } else {
		rs = SR_NOSYS ;
	    }
	} else {
	    rs = SR_INVALID ;
	} /* end if */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (ucsysconf::mconfstr) */

int ucsysconf::cache(int req) noex {
	int		rs = SR_OK ;
	int		ii = -1 ;
	switch (req) {
	case _SC_ARG_MAX: ii = dataitem_maxarg ; break ;
	case _SC_LINE_MAX: ii = dataitem_maxline ; break ;
	case _SC_HOST_NAME_MAX: ii = dataitem_maxhost ; break ;
	case _SC_LOGIN_NAME_MAX: ii = dataitem_maxlogin ; break ;
	default:
	    rs = (*this)(req) ;
	    break ;
	} /* end switch */
	if ((rs >= 0) && (ii >= 0)) {
	    if ((rs = ucdata.d[ii].load(memord_relaxed)) == 0) {
		if ((rs = (*this)(req)) > 0) {
		    ucdata.d[ii].store(rs,memord_relaxed) ;
		}
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (ucsysconf::cache) */



/* ucsysconf2 MODULE (module-implementation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* get system configuration information */
/* version %I% last-modified %G% */


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

	Returns:
	>=0		OK and this is the requested value
	<0		error (system-return)

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
	<0		error (system-return)

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>		/* |{xx}buflen| + |ustd_{x}(3u)| */
#include	<sysconfcmds.h>
#include	<localmisc.h>

module ucsysconf ;

/* local defines */


/* imported namespaces */

using ustd::ustd_confval ;		/* subroutine */
using ustd::ustd_confstr ;		/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */


/* local subroutines */

int ucsysconf::sysconfval(int req) noex {
    	int		rs ;
	switch (req) {
	case _SC_PID_MAX:
	case _SC_ARG_MAX:
	case _SC_LINE_MAX:
	case _SC_LOGIN_NAME_MAX:
	case _SC_NGROUPS_MAX:
	case _SC_NODENAME_MAX:
	case _SC_HOST_NAME_MAX:
	case _SC_MSG_MAX:
	case _SC_TZNAME_MAX:
	case _SC_CLK_TCK:
	    rs = getvalcache(req) ;
	    break ;
	default:
	    rs = getval(req) ;
	    break ;
	} /* end switch */
	return rs ;
} /* end method (ucsysconf::sysconfval) */

int ucsysconf::getval(int req) noex {
    	int		rs ;
	if (req >= sysconfcmd_synthetic) {
	    rs = getvalsyn(req) ;
	} else {
	    rs = getstd(req) ;
	}
	return rs ;
}

int ucsysconf::mconfval(int req) noex {
    	return ustd_confval(req,lp) ;
}
/* end subroutine (ucsysconf::mconfval) */

int ucsysconf::getvalcache(int req) noex {
	int		rs = SR_OK ;
	int		ii = -1 ;
	switch (req) {
	case _SC_PID_MAX:		ii = dataitem_maxpid ; break ;
	case _SC_ARG_MAX:		ii = dataitem_maxarg ; break ;
	case _SC_LINE_MAX:		ii = dataitem_maxline ; break ;
	case _SC_LOGIN_NAME_MAX:	ii = dataitem_maxlogin ; break ;
	case _SC_NODENAME_MAX:		ii = dataitem_maxnode ; break ;
	case _SC_HOST_NAME_MAX:		ii = dataitem_maxhost ; break ;
	case _SC_MSG_MAX:		ii = dataitem_maxmsg ; break ;
	case _SC_TZNAME_MAX:		ii = dataitem_maxtzname ; break ;
	case _SC_NGROUPS_MAX:		ii = dataitem_ngroups ; break ;
	case _SC_CLK_TCK:		ii = dataitem_clk ; break ;
	    break ;
	default:
	    rs = SR_BUGCHECK ;
	    break ;
	} /* end switch */
	if ((rs >= 0) && (ii >= 0)) {
	    if ((rs = ucdata.d[ii].load(memord_relaxed)) == 0) {
		if ((rs = getval(req)) > 0) {
		    ucdata.d[ii].store(rs,memord_relaxed) ;
		}
	    } else if (rs > 0) {
	        if (lp) *lp = long(rs) ;
	    }
	} /* end if */
	return rs ;
} /* end subroutine (ucsysconf::getvalcache) */

int ucsysconf::getvalsyn(int req) noex {
    	long		val = -1 ;
    	int		rs = SR_OK ;
	switch (req) {
	case sysconfcmd_maxpid:
	    val = PID_MAX ;
	    break ;
	case sysconfcmd_maxmsg:
	    val = MMBUFLEN ;
	    break ;
	case sysconfcmd_maxzoneinfo:
	    val = ZIBUFLEN ;
	    break ;
	default:
	    rs = SR_NOSYS ;
	    break ;
	} /* end switch */
	if (lp && (val >= 0)) {
	    *lp = (rs >= 0) ? val : 0L ;
	}
	if ((rs >= 0) && (val >= 0)) {
	    rs = intsat(val) ;
	}
	return rs ;
} /* end subroutine (ucsysconf::getvalsyn) */



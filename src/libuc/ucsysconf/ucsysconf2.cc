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
#include	<sys/utsname.h>		/* POSIX® |uname(2)| */
#include	<unistd.h>		/* POSIX® |_SC_{x}| */
#include	<utmpx.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<algorithm>		/* C++STD |sort(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<utimeout.h>		/* LIBU |uto_{x}| */
#include	<ustd.h>		/* LIBU |ustd_conf{x}| */
#include	<intsat.h>		/* LIBU */
#include	<sysconfcmds.h>		/* LIBU */
#include	<isoneof.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ucsysconf.h"

#pragma		GCC dependency		"mod/usysconf.ccm"

module ucsysconf ;

import usysconf ;

/* local defines */


/* imported namespaces */

using std::sort ;			/* subroutine-template */
using std::partition_point ;		/* subroutine-template */
using libu::ustd_confval ;		/* subroutine */
using libu::ustd_confstr ;		/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local bool	isNoSup(int) noex ;


/* local variables */

constexpr int	rsnosup[] = {
	SR_INVALID,
	SR_NOTSUP,
    	SR_NOSYS,
	0
} ; /* end array */

ucdatamgr		ucdata ;


/* exported variables */


/* exported subroutines */


/* local subroutines */

int ucsysconf::sysconfval(int req) noex {
    	cint		rsn = SR_NOTFOUND ;
    	int		rs ;
	if ((rs = getci(req)) >= 0) {
	    rs = getvalcache(req,rs) ;
	} else if (rs == rsn) {
	    rs = getval(req) ;
	} /* end if */
	return rs ;
} /* end method (ucsysconf::sysconfval) */

int ucsysconf::getvalcache(int req,int di) noex {
	int		rs ;
	    if ((rs = ucdata.d[di].load(memord_relaxed)) == 0) {
		if ((rs = getval(req)) > 0) {
		    ucdata.d[di].store(rs,memord_relaxed) ;
		}
	    } /* end if (filling cache) */
	    if (rs >= 0) {
	        if (lp) *lp = long(rs) ;
	    } /* end if (store value) */
	return rs ;
} /* end method (ucsysconf::getvalcache) */

int ucsysconf::getvalsyn(int req) noex {
    	long		val = -1 ;
    	int		rs = SR_OK ;
	switch (req) {
	case sysconfcmd_maxpid:
	    val = PID_MAX ;
	    break ;
        case sysconfcmd_maxline:
            val = MLBUFLEN ;
            break ;
	case sysconfcmd_maxlink:
	    val = LINK_MAX ;
	    break ;
	case sysconfcmd_maxlogin:
	    val = UNBUFLEN ;
	    break ;
	case sysconfcmd_maxgroups:
	    val = NGROUPS_MAX ;
	    break ;
	case sysconfcmd_maxsymloop:
	    val = SYMLOOP_MAX ;
	    break ;
	case sysconfcmd_maxsymbol:
	    val = SYMBOL_MAX ;
	    break ;
	case sysconfcmd_maxnodename:
	    val = NODENAME_MAX ;
	    break ;
	case sysconfcmd_maxname:
	    val = NAME_MAX ;
	    break ;
	case sysconfcmd_maxpath:
	    val = PATH_MAX ;
	    break ;
        case sysconfcmd_maxtzname:
            val = ZNBUFLEN ;
            break ;
	case sysconfcmd_maxzoneinfo:
	    val = ZIBUFLEN ;
	    break ;
        case sysconfcmd_maxusername:
            rs = UNBUFLEN ;
            break ;
        case sysconfcmd_maxgroupname:
            rs = GRBUFLEN ;
            break ;
        case sysconfcmd_maxprojectname:
            rs = PJBUFLEN ;
            break ;
        case sysconfcmd_maxprotname:
            val = PROTNAMELEN ;
            break ;
        case sysconfcmd_maxnetwname:
            val = NETWNAMELEN ;
            break ;
        case sysconfcmd_maxhostname:
            val = HOSTNAMELEN ;
            break ;
        case sysconfcmd_maxservname:
            val = SERVNAMELEN ;
            break ;
        case sysconfcmd_utent:
            val = szof(utmpx) ;
            break ;
        case sysconfcmd_pwent:
            val = PWBUFLEN ;
            break ;
        case sysconfcmd_spent:
            val = SPBUFLEN ;
            break ;
        case sysconfcmd_uaent:
            val = UABUFLEN ;
            break ;
        case sysconfcmd_grent:
            val = GRBUFLEN ;
            break ;
        case sysconfcmd_pjent:
            val = PJBUFLEN ;
            break ;
	case sysconfcmd_maxmsg:
	    val = MMBUFLEN ;
	    break ;
        case sysconfcmd_fstype:
            val = FSBUFLEN ;
            break ;
        case sysconfcmd_maxmailaddr:
            rs = usysconfval(_SC_MAILADDR_MAX) ;
            break ;
	default:
	    rs = SR_NOTSUP ;
	    break ;
	} /* end switch */
	if (rs >= 0) {
	    if (val < 0) val = rs ;
	    rs = intsat(val) ;
	} /* end if (ok) */
	if (lp) *lp = (rs >= 0) ? val : 0L ;
	return rs ;
} /* end method (ucsysconf::getvalsyn) */

int ucsysconf::getvaldef(int req) noex {
    	int		rs = SR_NOTSUP ;
	switch (req) {
	case _SC_TZNAME_MAX:
	    rs = 8 ;
	    break ;
	case _SC_ZONEINFO_MAX:
	    rs = 255 ;
	    break ;
	} /* end switch */
    	return rs ;
} /* end method (usysconf::getvaldef) */

int ucsysconf::getval(int req) noex {
    	cnullptr	np{} ;
    	int		rs ;
	int		val = 0 ; /* return-value */
	if ((rs = u_sysconfval(req,np)) >= 0) {
	    val = rs ;
	} else if (isNoSup(rs)) {
	    if (req >= sysconfcmd_synthetic) {
	        rs = getvalsyn(req) ;
	        val = rs ;
	    } else if ((rs = getstd(req)) >= 0) {
	        val = rs ;
	    } else if (isNoSup(rs)) {
	        rs = getvaldef(req) ;
	        val = rs ;
	    } /* end if */
	} /* end if (alternatives) */
	return (rs >= 0) ? val : rs ;
} /* end method (ucsysconf::getval) */

int ucsysconf::mconfval(int req) noex {
    	return ustd_confval(req,lp) ;
} /* end method (ucsysconf::mconfval) */

local bool isNoSup(int rs) noex {
    	return isOneBad(rsnosup,rs) ;
} /* end wubtoutine (isNoSup) */



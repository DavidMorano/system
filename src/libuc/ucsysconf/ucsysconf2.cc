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
#include	<sys/utsname.h>		/* POSIX |uname(2)| */
#include	<unistd.h>		/* POSIX */
#include	<utmpx.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<utimeout.h>		/* LIBU |uto_{x}| */
#include	<ustd.h>		/* LIBU |ustd_conf{x}| */
#include	<intsat.h>		/* LIBU */
#include	<sysconfcmds.h>		/* LIBU */
#include	<isoneof.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ucsysconf.h"

module ucsysconf ;

import usysconf ;

/* local defines */


/* imported namespaces */

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
    	SR_NOSYS,
	SR_NOTSUP,
	SR_INVALID,
	0
} ; /* end array */

ucdatamgr		ucdata ;	/* <- module linkage */


/* exported variables */


/* exported subroutines */


/* local subroutines */

int ucsysconf::sysconfval(int req) noex {
    	int		rs ;
	switch (req) {
        case _SC_PAGESIZE:
	case _SC_PID_MAX:
	case _SC_ARG_MAX:
	case _SC_LINE_MAX:
	case _SC_LINK_MAX:
	case _SC_LOGIN_NAME_MAX:	/* name */
	case _SC_NGROUPS_MAX:
        case _SC_SYMLOOP_MAX:
        case _SC_SYMBOL_MAX:		/* name */
        case _SC_NAME_MAX:
        case _SC_PATH_MAX:
	case _SC_NODENAME_MAX:		/* name */
        case _SC_USERNAME_MAX:		/* name */
        case _SC_GROUPNAME_MAX:		/* name */
        case _SC_PROJECTNAME_MAX:	/* name */
        case _SC_PROTNAME_MAX:		/* name */
        case _SC_NETWNAME_MAX:		/* name */
        case _SC_HOSTNAME_MAX:		/* name */
        case _SC_SERVNAME_MAX:		/* name */
	case _SC_UTMPENT_SIZE_MAX:	/* entry */
	case _SC_GETPW_R_SIZE_MAX:	/* entry */
	case _SC_GETSP_R_SIZE_MAX:	/* entry */
	case _SC_GETUA_R_SIZE_MAX:	/* entry */
	case _SC_GETGR_R_SIZE_MAX:	/* entry */
	case _SC_GETPJ_R_SIZE_MAX:	/* entry */
	case _SC_GETPR_R_SIZE_MAX:	/* entry */
	case _SC_GETNW_R_SIZE_MAX:	/* entry */
	case _SC_GETHO_R_SIZE_MAX:	/* entry */
	case _SC_GETSV_R_SIZE_MAX:	/* entry */
	case _SC_MSG_MAX:
        case _SC_FSTYPE:
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

int ucsysconf::getvalcache(int req) noex {
	int		rs = SR_OK ;
	int		ii = -1 ;
	switch (req) {
        case _SC_PAGESIZE:              ii = dataitem_pagesz ;          break ;
	case _SC_PID_MAX:		ii = dataitem_maxpid ;		break ;
	case _SC_ARG_MAX:		ii = dataitem_maxarg ;		break ;
	case _SC_LINE_MAX:		ii = dataitem_maxline ;		break ;
	case _SC_LINK_MAX:		ii = dataitem_maxlink ;		break ;
	case _SC_LOGIN_NAME_MAX:	ii = dataitem_maxlogin ;	break ;
	case _SC_NGROUPS_MAX:		ii = dataitem_maxgroups ;	break ;
        case _SC_SYMLOOP_MAX:           ii = dataitem_symlinks ;        break ;
        case _SC_SYMBOL_MAX:            ii = dataitem_maxsymbol ;       break ;
        case _SC_NAME_MAX:              ii = dataitem_maxnamelen ;      break ;
        case _SC_PATH_MAX:              ii = dataitem_maxpathlen ;      break ;
	case _SC_NODENAME_MAX:		ii = dataitem_maxnodename ;	break ;
        case _SC_USERNAME_MAX:          ii = dataitem_maxusername ;     break ;
        case _SC_GROUPNAME_MAX:         ii = dataitem_maxgroupname ;    break ;
        case _SC_PROJECTNAME_MAX:       ii = dataitem_maxprojectname ;  break ;
	case _SC_PROTNAME_MAX:		ii = dataitem_maxprot ;		break ;
        case _SC_NETWNAME_MAX:		ii = dataitem_maxnetw ;		break ;
	case _SC_HOSTNAME_MAX:		ii = dataitem_maxhost ;		break ;
	case _SC_SERVNAME_MAX:		ii = dataitem_maxserv ;		break ;
	case _SC_UTMPENT_SIZE_MAX:	ii = dataitem_maxentut ;	break ;
	case _SC_GETPW_R_SIZE_MAX:	ii = dataitem_maxentpw ;	break ;
	case _SC_GETSP_R_SIZE_MAX:	ii = dataitem_maxentsp ;	break ;
	case _SC_GETUA_R_SIZE_MAX:	ii = dataitem_maxentua ;	break ;
	case _SC_GETGR_R_SIZE_MAX:	ii = dataitem_maxentgr ;	break ;
	case _SC_GETPJ_R_SIZE_MAX:	ii = dataitem_maxentpj ;	break ;
	case _SC_GETPR_R_SIZE_MAX:	ii = dataitem_maxentpr ;	break ;
	case _SC_GETNW_R_SIZE_MAX:	ii = dataitem_maxentnw ;	break ;
	case _SC_GETHO_R_SIZE_MAX:	ii = dataitem_maxentho ;	break ;
	case _SC_GETSV_R_SIZE_MAX:	ii = dataitem_maxentsv ;	break ;
	case _SC_MSG_MAX:		ii = dataitem_maxmsg ;		break ;
        case _SC_FSTYPE:                ii = dataitem_maxfstype ;       break ;
	case _SC_TZNAME_MAX:		ii = dataitem_maxtzname ;	break ;
	case _SC_CLK_TCK:		ii = dataitem_clk ;		break ;
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
	    } /* end if (filling cache) */
	    if (rs >= 0) {
	        if (lp) *lp = long(rs) ;
	    } /* end if (store value) */
	} /* end if */
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
	case sysconfcmd_maxname:
	    val = NAME_MAX ;
	    break ;
	case sysconfcmd_maxpath:
	    val = PATH_MAX ;
	    break ;
	case sysconfcmd_maxnodename:
	    val = NODENAME_MAX ;
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
        case sysconfcmd_maxtzname:
            val = ZNBUFLEN ;
            break ;
	case sysconfcmd_maxzoneinfo:
	    val = ZIBUFLEN ;
	    break ;
        case sysconfcmd_maxmailaddr:
            rs = usysconfval(_SC_MAILADDR_MAX) ;
            break ;
	default:
	    rs = SR_NOSYS ;
	    break ;
	} /* end switch */
	if (rs >= 0) {
	    if (val < 0) val = rs ;
	    rs = intsat(val) ;
	} /* end if (ok) */
	if (lp) *lp = (rs >= 0) ? val : 0L ;
	return rs ;
} /* end method (ucsysconf::getvalsyn) */

int ucsysconf::getval(int req) noex {
    	int		rs ;
	int		val = 0 ;
	if (req >= sysconfcmd_synthetic) {
	    rs = getvalsyn(req) ;
	    val = rs ;
	} else if ((rs = getstd(req)) > 0) {
	    val = rs ;
	} else if ((rs < 0) && isNoSup(rs)) {
	    rs = getvalsyn(req) ;
	    val = rs ;
	} /* end if */
	return (rs >= 0) ? val : rs ;
} /* end method (ucsysconf::getval) */

int ucsysconf::mconfval(int req) noex {
    	return ustd_confval(req,lp) ;
} /* end method (ucsysconf::mconfval) */

local bool isNoSup(int rs) noex {
	return isOneOf(rsnosup,rs) ;
} /* end wubtoutine (isNoSup) */



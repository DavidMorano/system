/* usysconf1 MODULE (module-implementation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* get system configuration information */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

	= 2020-05-07, David A-D- Morano
	I converted this (formerly a header-only file) to a module.

*/

/* Copyright © 1998,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	usysconfval

	Description:
	This subroutine returns system-related values (as opposed
	to strings as |usysconfstr(3uc)| does).

	Synopsis:
	int usysconfval(int req,long *rp) noex

	Arguments:
	req		requested value to return
	rp		pointer to value-type |long| for received value

	Returns:
	>=0		OK and this is the requested value
	<0		error (system-return)

	Name:
	usysconfstr

	Description:
	This subroutine is NOT the same as the UNIX®-System version.
	This subroutine returns "OVERFLOW" when the user supplied
	buffer is not big enough to hold the associated value.

	Synopsis:
	int usysconfstr(int req,char *rbuf,int rlen) noex

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
#include	<unistd.h>		/* POSIX |sysconf| stuff */
#include	<utmpx.h>		/* POSIX UTMPX */
#include	<cerrno>		/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD |nothrow(3c++)| */
#include	<atomic>		/* C++STD |atomic_int(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<utimeout.h>		/* LIBU */
#include	<ustd.h>		/* LIBU converted system calls */
#include	<usys.h>		/* LIBU |umaxmsglen(3u)| */
#include	<usysflag.h>		/* LIBU */
#include	<intsat.h>		/* LIBU */
#include	<errtimer.hh>		/* LIBU */
#include	<sysconfcmds.h>		/* LIBU */
#include	<mailvalues.hh>		/* LIBU |mailvalue(3u)| */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

module usysconf ;

import usysconfcheck ;			/* cache item check */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */

using std::atomic_int ;			/* type */
using libu::umaxmsglen ;		/* subroutine */
using libu::ustd_confval ;		/* subroutine */
using libu::ustd_confstr ;		/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct ucachestore {
	atomic_int	d[dataitem_overlast] ;
    } ; /* end struct (ucachestore) */
    struct usysconfmgr ;
    typedef int (usysconfmgr::*usysconfmgr_m)(int) ;
    struct usysconfmgr {
	usysconfmgr_m	m ;
	char		*rbuf ;
	long		*lp ;
	int		rlen ;
	usysconfmgr(char *rp,int rl) noex : rbuf(rp), rlen(rl) { } ;
	usysconfmgr(long *p) noex : lp(p) { } ;
	int mconfval	(int) noex ;
	int mconfstr	(int) noex ;
	int getval	(int) noex ;
	int getstr	(int) noex ;
	int getvalcache	(int,int) noex ;
	int getvalsys	(int) noex ;
	int getvaldef	(int) noex ;
	int getvalsyn	(int) noex ;
	int callstd	(int) noex ;
	int getdefmsg	() noex ;
	int getdefzoneinfo() noex ;
	int getdefacctname() noex ;
	int getdefnodename() noex ;
	int getdefmailaddr() noex ;
    } ; /* end struct (usysconf) */
} /* end namespace */


/* forward references */

local inline bool isNoSup(int) noex ;


/* local variables */

constexpr int		rsnosup[] = {
    	SR_INVALID,
	SR_NOTSUP,
	SR_NOSYS
} ; /* end array */

static ucachestore	udata ;
constexpr size_t	minusone	= -1uz ;
cbool			f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

extern "C" {
    int u_sysconfval(int cmd,long *rp) noex {
        return usysconfval(cmd,rp) ;
    } /* end */
    int u_sysconfstr(int cmd,char *rbuf,int rlen) noex {
        return usysconfstr(cmd,rbuf,rlen) ;
    } /* end */
} /* end extern (C) */


extern "C++" {
    int usysconfval(int req,long *rp) noex {
	int		rs = SR_INVALID ;
	DPRINTF("ent\n") ;
	if (req >= 0) {
	    usysconfmgr	sco(rp) ;
	    sco.m = &usysconfmgr::mconfval ;
	    rs = sco.getval(req) ;
	} /* end if (valid) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
    } /* end subroutine (usysconfval) */
    int usysconfstr(int req,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    rs = SR_INVALID ;
	    if ((req >= 0) && (rlen >= 0)) {
	        usysconfmgr	sco(rbuf,rlen) ;
	        sco.m = &usysconfmgr::mconfstr ;
	        rs = sco.getstr(req) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (usysconfstr) */
} /* end extern (C++) */


/* local subroutines */

int usysconfmgr::getvalsys(int req) noex {
    	int		rs ;
	int		val = 0 ; /* return-value */
	if (req >= sysconfcmd_synthetic) {
	    rs = getvalsyn(req) ;
	    val = rs ;
	} else {
	    if ((rs = callstd(req)) >= 0) {
		val = rs ;
	    } else if (isNoSup(rs)) {
		rs = getvaldef(req) ;
		val = rs ;
	    } /* end if */
	} /* end if */
    	return (rs >= 0) ? val : rs ;
} /* end subroutine (usysconfmgr::getvalsys) */

int usysconfmgr::getstr(int req) noex {
    	return callstd(req) ;
} /* end subroutine (usysconfmgr::getstr) */

int usysconfmgr::getval(int req) noex {
        cint            rsn = SR_NOTFOUND ;
    	int		rs ;
	DPRINTF("ent\n") ;
        if ((rs = getci(req)) >= 0) {
            rs = getvalcache(req,rs) ;
        } else if (rs == rsn) {
	    rs = getvalsys(req) ;
	} /* end switch */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (usysconfmgr::getval) */

int usysconfmgr::getvalcache(int req,int ci) noex {
	int		rs ;
	DPRINTF("ent req=%d ci=%dn",req,ci) ;
	if ((rs = udata.d[ci].load(memord_relaxed)) == 0) {
	    if ((rs = getvalsys(req)) > 0) {
		udata.d[ci].store(rs,memord_relaxed) ;
	    }
	} /* end if (filling cache) */
	if (rs >= 0) {
	    if (lp) *lp = long(rs) ;
	} /* end if (result) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (usysconfmgr::getvalcache) */

int usysconfmgr::getvalsyn(int req) noex {
    	long		val = -1 ;
    	int		rs = SR_OK ;
	DPRINTF("ent req=%d\n",req) ;
	switch (req) {
        case sysconfcmd_maxpid:		/* maximum PID value */
	    val = PID_MAX ;		/* six '9's is the common standard */
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
	    val = SNBUFLEN ;
	    break ;
        case sysconfcmd_maxnodename:
	    rs = getdefnodename() ;
	    break ;
        case sysconfcmd_maxname:
	    val = MNBUFLEN ;
	    break ;
        case sysconfcmd_maxpath:
	    val = MPBUFLEN ;
	    break ;
        case sysconfcmd_maxtzname:
	    val = ZNBUFLEN ;
	    break ;
        case sysconfcmd_maxtzabbr:
	    val = ZABUFLEN ;
	    break ;
        case sysconfcmd_maxzoneinfo:
	    rs = getdefzoneinfo() ;
	    break ;
        case sysconfcmd_maxusername:
	    rs = getdefacctname() ;
	    break ;
        case sysconfcmd_maxgroupname:
	    rs = getdefacctname() ;
	    break ;
        case sysconfcmd_maxprojectname:
	    rs = getdefacctname() ;
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
	    rs = getdefmsg() ;
	    break ;
	case sysconfcmd_fstype:
	    val = FSBUFLEN ;
	    break ;
        case sysconfcmd_maxmailaddr:
	    rs = getdefmailaddr() ;
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
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (usysconfmgr::getvalsyn) */

int usysconfmgr::getvaldef(int req) noex {
    	int		rs = SR_NOTSUP ;
	switch (req) {
	case _SC_TZNAME_MAX:
	    rs = 8 ;
	    break ;
	case _SC_ZONEINFO_MAX :
	    rs = 255 ;
	    break ;
	} /* end switch */
	return rs ;
} /* end method (usysconfmgr::getvaldef) */

int usysconfmgr::callstd(int req) noex {
	errtimer	to_again	= utimeout[uto_again] ;
	errtimer	to_busy		= utimeout[uto_busy] ;
	errtimer	to_nomem	= utimeout[uto_nomem] ;
	reterr		r ;
	int		rs ;
	DPRINTF("ent\n") ;
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
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (usysconfmgr::callstd) */

int usysconfmgr::mconfval(int req) noex {
    	return ustd_confval(req,lp) ;
} /* end subroutine (usysconfmgr::mconfval) */

int usysconfmgr::mconfstr(int req) noex {
    	return ustd_confstr(req,rbuf,rlen) ;
} /* end subroutine (usysconfmgr::mconfstr) */

int usysconfmgr::getdefmsg() noex {
	int		rs ;
	if (usysflag.darwin) {
	    rs = umaxmsglen() ;
	} else {
	    rs = MMBUFLEN ;		/* Maximum-Message-Buffer-Length */
	} /* end if_constexpr (usysflag.darwin) */
	return rs ;
} /* end method (usysconfmgr::getdefmsg) */

int usysconfmgr::getdefzoneinfo() noex {
	cint		cmdname = _SC_NAME_MAX ;
	return getval(cmdname) ;
} /* end method (usysconfmgr::getdefzoneinfo) */

int usysconfmgr::getdefacctname() noex {
	cint		cmdlogin = _SC_LOGIN_NAME_MAX ;
	return getval(cmdlogin) ;
} /* end method (usysconfmgr::getdefacctname) */

int usysconfmgr::getdefnodename() noex {
    	UTSNAME		ut ;
	int		rs ;
	{
	    rs = (szof(ut.nodename) - 1) ;
	}
	return rs ;
} /* end method (usysconfmgr::getdefnodename) */

int usysconfmgr::getdefmailaddr() noex {
	cint		cmdnode = _SC_NODENAME_MAX ;
	cint		cmdhost = _SC_HOSTNAME_MAX ;
    	cint		nnm = mailvalue.nodenamemult ;
    	cint		hnm = mailvalue.hostnamemult ;
	int		rs ;
	int		len = 0 ; /* return-value */
	if ((rs = getval(cmdnode)) >= 0) {
	    len += (rs * nnm) ;
	    if ((rs = getval(cmdhost)) >= 0) {
    		len += (rs * hnm) ;
	    }
	} /* end if (getval) */
    	return (rs >= 0) ? len : rs ;
} /* end method (usysconfmgr::getdefmailaddr) */

local inline bool isNoSup(int rs) noex {
    	bool f = false ;
	if (rs < 0) {
	    for (cauto &e : rsnosup) {
		if ((f = (rs == e))) break ;
	    } /* end for */
	} /* end if (pssible) */
	return f ;
} /* end subroutine (isNoSup) */



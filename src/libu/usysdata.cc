/* usysdata SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++23 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	u_uname
	u_getnodename
	u_getauxinfo
    	libu::loadhwserial
    	libu::ugethostid

	Description:
	Retrieve some operating system and machine information.

	Synopsis:
	int u_uname		(utsname *up) noex
	int u_getnodename	(char *rbuf,int rlen) noex
	int u_getauxinfo	(char *rbuf,int rlen,int req) noex
	int u_gethostid		(ulong *) noex
    	int libu::loadhwserial	(char *,int) noex
    	int libu::ugethostid	(ulong *) noex

	Arguments:
	up		UTSNAME object pointer
	rbuf		result buffer pointer
	rlen		result buffer length
	req		request code

	Returns:
	>=0		OK
	<0		error (system-return)

	Notes:
	Yes, a good bit (most of) the stuff below is the caching
	mechanism for four of the possible requests.  Those four
	requests are for information that will not (cannot) change
	during the entire time that the operating system is running.
	So, why not cache those values?  On some platforms and
	operating systems (like possibly on Apple Darwin) there can
	be a substantial time savings in caching some of these
	values rather trying to extract them from the OS everytime
	they are requested.  And YES, I recognize that if every
	caller used the (so-called) UCINFO interface (see |ucinfo(3uc)|)
	these values are even more cached and accessable through
	that interface than they even are through this present
	(below) caching interface.  I say, the more the merrier!
	Currently, or should I say originally (at the time of this
	original writing), I am caching the following four request
	values:
	    usysauxinforeq_architecture
	    usysauxinforeq_machine
	    usysauxinforeq_platform
	    usysauxinforeq_hwprovider
	Enjoy!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/utsname.h>		/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strcmp(3c)| */
#include	<new>			/* C++STD |nothrow(3c++)| */
#include	<utility>		/* C++STD |unreachable(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usysflag.h>		/* LIBU */
#include	<usupport.h>		/* LIBU <- most of |libu| namespace */
#include	<usysauxinfo.h>		/* LIBU the request codes */
#include	<usyscallbase.hh>	/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"usysdata.h"

import ulibvals ;			/* |ulibval(3u)| */

/* local defines */


/* imported namespaces */

using libu::usyscallbase ;		/* type */
using libu::ugethostid ;		/* subroutine */
using libu::sncpy ;			/* subroutine */
using libu::snwcpy ;			/* subroutine */
using libu::strwcpy ;			/* subroutine */
using usysauxinfo::ugetauxinfo ;	/* subroutine */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef int (*uname_f)(utsname *) noex ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct syscaller ;
    typedef int (syscaller::*syscaller_m)() noex ;
    struct syscaller : usyscallbase {
	syscaller_m	m = nullptr ;
	utsname		*utsp ;
	ulong		*idp ;
	int operator () (utsname *p) noex {
	    utsp = p ;
	    return handler() ;
	} ;
	int operator () (ulong *p) noex {
	    idp = p ;
	    return handler() ;
	} ;
        int callstd() noex override {
            int         rs = SR_BUGCHECK ;
            if (m) {
                rs = (this->*m)() ;
            }
            return rs ;
        } ; /* end method (callstd) */
	int std_uname() noex ;
	int std_gethostid() noex ;
    } ; /* end struct (syscaller) */
} /* end namespace */

namespace {
    constexpr int	nitems = 4 ;
    struct umachiner {
	cchar		*architecture ;
	cchar		*machine ;
	cchar		*platform ;
	cchar		*hwprovider ;
	destruct umachiner() {
	    delete mbuf ;
	    mbuf = nullptr ;
	    mlen = 0 ;
 	} ; /* end destruct (memory-release) */
	int setup() noex ;
    private:
	char		*mbuf ;
	int		mlen ;
    } ; /* end struct (umachiner) */
    struct datobj {
	char		*s[nitems] ;
	char		*a = nullptr ;
	int start() noex ;
	int finish() noex ;
	int load() noex ;
	destruct datobj() {
	    (void) finish() ;
 	} ;
    } ; /* end struct (datobj) */
} /* end namespace */


/* forward references */

local sysret_t usys_uname	(utsname *) noex ;
local sysret_t uname_machine	(utsname *) noex ;
local sysret_t uname_nodename	(utsname *) noex ;
local sysret_t local_getauxinfo	(char *,int,int) noex ;
local sysret_t setup_sysauxinfo	() noex ;


/* local variables */

constexpr uname_f	usubs[] = {
	uname_machine,
	uname_nodename
} ; /* end array */

static umachiner	um ;

constexpr int		reqs[] = {
	SAI_ARCHITECTURE,
	SAI_MACHINE,
	SAI_PLATFORM,
	SAI_HWPROVIDER
} ; /* end array */

static cint		datlen = ulibval.nodenamelen ;

constexpr cchar		defmachine[] = "Intel(R) Core(TM) i7" ;

constexpr cbool		f_sunos		= F_SUNOS ;
constexpr cbool		f_darwin	= F_DARWIN ;
constexpr cbool		f_linux		= F_LINUX ;


/* exported variables */


/* exported subroutines */

int u_uname(utsname *up) noex {
	int		rs = SR_FAULT ;
	int		rc = 0 ;
	if (up) ylikely {
	    if ((rs = usys_uname(up)) >= 0) ylikely {
		rc = rs ;
		for (cauto &f : usubs) {
		    if ((rs = f(up)) < 0) break ;
		} /* end for */
	    } /* end if (usys_uname) */
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
} /* end subroutine (u_uname) */

int u_getnodename(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf) ylikely {
	    rs = SR_NOMEM ;
	    if (utsname *utsp = new(nothrow) utsname ; utsp) ylikely {
		if ((rs = u_uname(utsp)) >= 0) {
	            rs = sncpy(rbuf,rlen,utsp->nodename) ;
		    len = rs ;
		}
	        delete utsp ;
	    } /* end if (utsname) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (u_uname) */

int u_getauxinfo(char *rbuf,int rlen,int req) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf) ylikely {
	    switch (req) {
	    case usysauxinforeq_architecture:
	    case usysauxinforeq_machine:
	    case usysauxinforeq_platform:
	    case usysauxinforeq_hwprovider:
		rs = local_getauxinfo(rbuf,rlen,req) ;
		len = rs ;
		break ;
	    default:
		rs = ugetauxinfo(rbuf,rlen,req) ;
		len = rs ;
		break ;
	    } /* end switch */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (u_getauxinfo) */

int u_gethostid(ulong *idp) noex {
	return ugethostid(idp) ;
} /* end subroutine (u_getauxinfo) */

namespace libu {
    sysret_t ugethostid(ulong *idp) noex {
	int		rs = SR_FAULT ;
	if (idp) ylikely {
	    syscaller	sc ;
	    sc.m = &syscaller::std_gethostid ;
	    rs = sc(idp) ;
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (ugethostid) */
    sysret_t loadhwserial(char *dp,int dl) noex {
	int		rs = SR_FAULT ;
	if (dp) ylikely {
	    if (ulong hid ; (rs = ugethostid(&hid)) >= 0) ylikely {	
		rs = ctdec(dp,dl,hid) ;
	    }
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (loadhwserial) */
} /* end namespace (libu) */


/* local subroutines */

local sysret_t usys_uname(utsname *utsp) noex {
	syscaller	sc ;
	sc.m = &syscaller::std_uname ;
	return sc(utsp) ;
} /* end subroutine (usys_uname) */

local sysret_t uname_machine(utsname *up) noex {
	cint		mlen = (szof(up->machine) - 1) ;
	int		rs = SR_OK ;
	char		*mbuf = up->machine ;
	if (strcmp(mbuf,"x86_64") == 0) {
	    if_constexpr (f_darwin) {
		cint	req = SAI_MACHINE ;
	        rs = ugetauxinfo(mbuf,mlen,req) ;
	    } else {
	        rs = sncpy(mbuf,mlen,defmachine) ;
	    }
	} /* end if (compared equal) */
	return rs ;
} /* end subroutine (uname_machine) */

local sysret_t uname_nodename(utsname *up) noex {
	int		rs = SR_OK ;
	char		*nn = up->nodename ;
	if (char *tp = strchr(nn,'.') ; tp) {
	    *tp = '\0' ;
	}
	return rs ;
} /* end subroutine (uname_nodename) */

local sysret_t local_getauxinfo(char *rbuf,int rlen,int req) noex {
	static cint	rsx = setup_sysauxinfo() ;
	int		rs ;
	int		len = 0 ;
	if ((rs = rsx) >= 0) ylikely {
	    cchar	*valp = nullptr ;
	    switch (req) {
	    case usysauxinforeq_architecture:
		valp = um.architecture ;
		break ;
	    case usysauxinforeq_machine:
		valp = um.machine ;
		break ;
	    case usysauxinforeq_platform:
		valp = um.platform ;
		break ;
	    case usysauxinforeq_hwprovider:
		valp = um.hwprovider ;
		break ;
	    } /* end switch */
	    if (valp) {
		rs = sncpy(rbuf,rlen,valp) ;
		len = rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine */

local sysret_t setup_sysauxinfo() noex {
	return um.setup() ;
} /* end subrooutine */

int umachiner::setup() noex {
	int		rs ;
	int		rs1 ;
	if (datobj dob ; (rs = dob.start()) >= 0) ylikely {
	    if ((rs = dob.load()) >= 0) ylikely {
		mlen = rs ;
		rs = SR_NOMEM ;
		if ((mbuf = new(nothrow) char[mlen+1]) != nullptr) {
		    char	*bp = mbuf ;
		    rs = SR_OK ;
		    for (int i = 0 ; i < nitems ; i += 1) {
			cchar	*sp = dob.s[i] ;
			switch (i) {
			case 0:
			    architecture = bp ;
			    break ;
			case 1:
			    machine = bp ;
			    break ;
			case 2:
			    platform = bp ;
			    break ;
			case 3:
			    hwprovider = bp ;
			    break ;
			default:
			    std::unreachable() ; /* <- stupid compiler */
			} /* end switch */
			bp = (strwcpy(bp,sp) + 1) ;
		    } /* end for */
		} /* end if (memory-acquire) */
	        rs1 = dob.finish() ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (dob.load) */
	} /* end if (dataobj) */
	return rs ;
} /* end method (umachiner::setup) */

int datobj::start() noex {
	cnullptr	np{} ;
	cint		sz = ((datlen + 1) * nitems) ;
	int		rs = SR_NOMEM ;
	if ((a = new(nothrow) char[sz]) != np) ylikely {
	    cint	n = nitems ;
	    rs = SR_OK ;
	    for (int i = 0 ; i < n ; i += 1) {
	        s[i] = (a + (i * (datlen + 1))) ;
	    }
	} /* end if (new-char) */
	return rs ;
} /* end method (datobj::start) */

int datobj::finish() noex {
	int		rs = SR_NOTOPEN ;
	if (a) {
	    delete [] a ;
	    a = nullptr ;
	    rs = SR_OK ;
	} /* end if (memory-release) */
	return rs ;
} /* end method (datobj::finish) */

int datobj::load() noex {
	cint		n = nitems ;
	int		rs = SR_OK ;
	int		rsz = 0 ;
	for (int i = 0 ; (rs >= SR_OK) && (i < n) ; i += 1) {
	    rs = ugetauxinfo(s[i],datlen,reqs[i]) ;
	    rsz += (rs + 1) ;
	} /* end for */
	return (rs >= 0) ? rsz : rs ;
} /* end method (datobj::load) */

int syscaller::std_uname() noex {
	int		rs ;
	if ((rs = uname(utsp)) < 0) {
	    rs = (neg errno) ;
	}
	return rs ;
} /* end method (syscaller::std_uname) */

int syscaller::std_gethostid() noex {
	int		rs = SR_OK ;
	if (long res ; (res = gethostid()) >= 0) ylikely {
	    *idp = ulong(res) ;
	} else {
	    rs = (neg errno) ;
	}
	return rs ;
} /* end method (syscaller::std_gethostid) */



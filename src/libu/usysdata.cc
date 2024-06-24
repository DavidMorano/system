/* usysdata SUPPORT */
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

	Description:
	Retrieve some operating system and machine information.

	Synopsis:
	int u_uname(utsname *up) noex
	int u_getnodename(char *rbuf,int rlen) noex
	int u_getauxinfo(char *rbuf,int rlen,int req) noex

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
	caller used the (so-called) UINFO interface (see |uinfo(3uc)|)
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
#include	<sys/types.h>
#include	<sys/utsname.h>
#include	<climits>		/* |INT_MAX| */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>		/* <- for |strcmp(3c)| */
#include	<utility>		/* |unreachable(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<usysflag.h>
#include	<usupport.h>		/* <- most of |libu| namespace */
#include	<usysauxinfo.h>		/* the request codes */
#include	<usyscallbase.hh>
#include	<localmisc.h>		/* |NODENAMELEN| */

#include	"usysdata.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
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
	int operator () (utsname *p) noex {
	    utsp = p ;
	    return handler() ;
	} ;
        int callstd() noex override {
            int         rs = SR_BUGCHECK ;
            if (m) {
                rs = (this->*m)() ;
            }
            return rs ;
        } ;
	int std_uname() noex ;
    } ; /* end struct (syscaller) */
}

namespace {
    constexpr int	nitems = 4 ;
    struct umachiner {
	cchar		*architecture ;
	cchar		*machine ;
	cchar		*platform ;
	cchar		*hwprovider ;
	~umachiner() {
	    delete mbuf ;
	    mbuf = nullptr ;
	    mlen = 0 ;
 	} ;
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
	~datobj() {
	    (void) finish() ;
 	} ;
    } ; /* end struct (datobj) */
}


/* forward references */

static int usys_uname(utsname *) noex ;
static int uname_machine(utsname *) noex ;
static int uname_nodename(utsname *) noex ;
static int local_getauxinfo(char *,int,int) noex ;
static int setup_sysauxinfo() noex ;


/* local variables */

static constexpr uname_f	usubs[] = {
	uname_machine,
	uname_nodename
} ;

static umachiner	um ;

constexpr int		reqs[] = {
	SAI_ARCHITECTURE,
	SAI_MACHINE,
	SAI_PLATFORM,
	SAI_HWPROVIDER
} ;

constexpr int		datlen = NODENAMELEN ;

constexpr cchar		defmachine[] = "Intel(R) Core(TM) i7" ;

constexpr bool		f_sunos		= F_SUNOS ;
constexpr bool		f_darwin	= F_DARWIN ;
constexpr bool		f_linux		= F_LINUX ;


/* exported variables */


/* exported subroutines */

int u_uname(utsname *up) noex {
	int		rs = SR_FAULT ;
	int		rc = 0 ;
	if (up) {
	    if ((rs = usys_uname(up)) >= 0) {
		rc = rs ;
		for (cauto &f : usubs) {
		    if ((rs = f(up)) < 0) break ;
		} /* end for */
	    } else {
	        rs = (- errno) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (u_uname) */

int u_getnodename(char *rbuf,int rlen) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf) {
	    rs = SR_NOMEM ;
	    if (utsname *utsp ; (utsp = new(nothrow) utsname) != np) {
		if ((rs = u_uname(utsp)) >= 0) {
	            rs = sncpy(rbuf,rlen,utsp->nodename) ;
		    len = rs ;
		}
	        delete utsp ;
	    } /* end if (new-utsname) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (u_uname) */

int u_getauxinfo(char *rbuf,int rlen,int req) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf) {
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
}
/* end subroutine (u_getauxinfo) */

int u_gethostid(ulong *idp) noex {
	return ugethostid(idp) ;
}
/* end subroutine (u_getauxinfo) */


/* local subroutines */

static int usys_uname(utsname *utsp) noex {
	syscaller	sc ;
	sc.m = &syscaller::std_uname ;
	return sc(utsp) ;
}
/* end subroutine (usys_uname) */

static int uname_machine(utsname *up) noex {
	cint		mlen = int(sizeof(up->machine)-1) ;
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
}
/* end subroutine (uname_machine) */

static int uname_nodename(utsname *up) noex {
	int		rs = SR_OK ;
	char		*nn = up->nodename ;
	if (char *tp ; (tp = strchr(nn,'.')) != nullptr) {
	    *tp = '\0' ;
	}
	return rs ;
}
/* end subroutine (uname_nodename) */

static int local_getauxinfo(char *rbuf,int rlen,int req) noex {
	static cint	rsx = setup_sysauxinfo() ;
	int		rs ;
	int		len = 0 ;
	if ((rs = rsx) >= 0) {
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
}

static int setup_sysauxinfo() noex {
	return um.setup() ;
}

int umachiner::setup() noex {
	datobj		dob ;
	int		rs ;
	int		rs1 ;
	if ((rs = dob.start()) >= 0) {
	    if ((rs = dob.load()) >= 0) {
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
		} /* end if (memory-allocation) */
	        rs1 = dob.finish() ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (dob.load) */
	} /* end if (dataobj) */
	return rs ;
}
/* end method (umachiner::setup) */

int datobj::start() noex {
	cnullptr	np{} ;
	cint		sz = ((datlen + 1) * nitems) ;
	int		rs = SR_NOMEM ;
	if ((a = new(nothrow) char[sz]) != np) {
	    cint	n = nitems ;
	    rs = SR_OK ;
	    for (int i = 0 ; i < n ; i += 1) {
	        s[i] = (a + (i * (datlen + 1))) ;
	    }
	} /* end if (new-char) */
	return rs ;
}
/* end method (datobj::start) */

int datobj::finish() noex {
	int		rs = SR_NOTOPEN ;
	if (a) {
	    delete [] a ;
	    a = nullptr ;
	    rs = SR_OK ;
	}
	return rs ;
}
/* end method (datobj::finish) */

int datobj::load() noex {
	cint		n = nitems ;
	int		rs = SR_OK ;
	int		rsz = 0 ;
	for (int i = 0 ; (rs >= SR_OK) && (i < n) ; i += 1) {
	    rs = ugetauxinfo(s[i],datlen,reqs[i]) ;
	    rsz += (rs + 1) ;
	} /* end for */
	return (rs >= 0) ? rsz : rs ;
}
/* end method (datobj::load) */

int syscaller::std_uname() noex {
	int		rs ;
	if ((rs = uname(utsp)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}		
/* end method (syscaller::std_uname) */



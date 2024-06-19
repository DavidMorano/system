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

	Name:
	u_uname
	u_getauxinfo

	Description:
	Retrieve some operating system and machine information.

	Synopsis:
	int u_uname(UTSNAME *up) noex
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
	be a substantial savings in caching some of these values
	rather trying to extract them from the OS everytime they
	are requested.  And YES, I recognize that if every caller
	used the (so-called) UINFO interface (see |uinfo(3uc)|)
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
#include	<localmisc.h>		/* |REALNAMELEN| */

#include	"usysdata.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using libu::sncpy ;			/* subroutine */
using libu::snwcpy ;			/* subroutine */
using libu::strwcpy ;			/* subroutine */
using usysauxinfo::ugetauxinfo ;	/* subroutine */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef int (*uname_f)(UTSNAME *) noex ;


/* external subroutines */


/* external variables */


/* local structures */

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
	char		*mbuf = nullptr ;
	int		mlen ;
    } ; /* end struct (umachiner) */
    struct datobj {
	char		*s[nitems] ;
	char		*a ;
	int start() noex ;
	int finish() noex ;
	int load() noex ;
	~datobj() {
	    (void) finish() ;
 	} ;
    } ; /* end struct (datobj) */
}


/* forward references */

static int local_getauxinfo(char *,int,int) noex ;
static int uuname_machine(UTSNAME *) noex ;
static int setup_sysauxinfo() noex ;


/* local variables */

static constexpr uname_f	usubs[] = {
	uuname_machine,
	nullptr
} ;

static umachiner	um ;

constexpr int		reqs[] = {
	SAI_ARCHITECTURE,
	SAI_MACHINE,
	SAI_PLATFORM,
	SAI_HWPROVIDER
} ;

constexpr int		datlen = REALNAMELEN ;

constexpr cchar		defmachine[] = "Intel(R) Core(TM) i7" ;

constexpr bool		f_sunos		= F_SUNOS ;
constexpr bool		f_darwin	= F_DARWIN ;
constexpr bool		f_linux		= F_LINUX ;


/* exported variables */


/* exported subroutines */

int u_uname(UTSNAME *up) noex {
	int		rs = SR_FAULT ;
	int		rc = 0 ;
	if (up) {
	    if ((rs = uname(up)) >= 0) {
		rc = rs ;
		for (int i = 0 ; (rs >= 0) && usubs[i] ; i += 1) {
		    uname_f	func = usubs[i] ;
		    rs = func(up) ;
		} /* end for */
	    } else {
	        rs = (- errno) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
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


/* local subroutines */

static int uuname_machine(UTSNAME *up) noex {
	cint		mlen = (sizeof(up->machine)-1) ;
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
		rs = snwcpy(rbuf,rlen,valp) ;
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



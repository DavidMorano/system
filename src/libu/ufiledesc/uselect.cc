/* uselect SUPPORT (UNIX®-At-Xntthing) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	u_select

	Description:
	Thes perform some access-permisson deterinations.

	Synopsis:
	int u_select(int nfds,fdset *ifds,fdset *ofds,fdset *efds) noex

	Arguments:
	nfds		maximum number of file-descriptors set
	ifds		file-descriptors input
	ofds		file-descriptors output
	efds		file-descriptors error

	Returns:
	>=0		number of descriptors with activity
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<localmisc.h>

#include	"ufiledescbase.hh"
#include	"uselect.h"


/* local defines */

#define	SET	fdset
#define	TV	TIMEVAL


/* imported namespaces */

using namespace usys ;			/* namespace */

using libu::ufiledescbase ;		/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct selector ;
    typedef int (selector::*selector_m)(int) noex ;
    struct selector : ufiledescbase {
	selector_m	m ;
	int		nfds ;
	fdset		*ifds ;
	fdset		*ofds ;
	fdset		*efds ;
	TIMEVAL		*tvp ;
	selector(selector_m µm,int n,SET *i,SET *o,SET *e,TV *tp) noex {
	    m = µm ;
	    nfds = n ;
	    ifds = i ;
	    ofds = o ;
	    efds = e ;
	    tvp = tp ;
	} ; /* end ctor */
	int callstd(int fd) noex override final {
	    int		rs = SR_BUGCHECK ;
	    if (m) {
		rs = (this->*m)(fd) ;
	    }
	    return rs ;
	} ; /* end method */
	sysret_t iselect(int) noex ;
    } ; /* end struct (selector) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace libu {
    int uselect(int nfds,SET *ifds,SET *ofds,SET *efds,TV *tvp) noex {
	selector fo(&selector::iselect,nfds,ifds,ofds,efds,tvp) ;
	return fo(0) ;
    } /* end subroutine (uselect) */
} /* end namespace libu) */

int u_select(int nfds,SET *ifds,SET *ofds,SET *efds,TV *tvp) noex {
    	using		libu::uselect ;
	int		rs = SR_INVALID ;
	if (nfds > 0) {
	    rs = uselect(nfds,ifds,ofds,efds,tvp) ;
	} /* end if (valid) */
	return rs ;
} /* end subroutine (u_select) */


/* local subroutines */

sysret_t selector::iselect(int) noex {
    	int		rs ;
	if ((rs = select(nfds,ifds,ofds,efds,tvp)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
} /* end method (selector::iselect) */



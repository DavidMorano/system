/* uatx SUPPORT */
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
	u_ataccess
	u_atperm

	Description:
	Thes perform some access-permisson deterinations.

	Synopsis:
	int u_ataccess(int fd,cchar *fn,int am,int fl) noex
	int u_atperm(int fd,cchar *fn,int am) noex

	Returns:
	>=0		size of data returned
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
#include	"uatx.h"


/* local defines */


/* imported namespaces */

using namespace usys ;			/* namespace */

using libu::ufiledescbase ;		/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct uatxer ;
    typedef int (uatxer::*uatxer_m)(int) noex ;
    struct uatxer : ufiledescbase {
	uatxer_m	m ;
	cchar		*fn ;
	int		am ;
	int		fl ;
	uatxer(uatxer_m µm,cchar *n,int a,int f = -1) noex : m(µm), fn(n) { 
	    am = a ;
	    fl = f ;
	} ; /* end ctor */
	int callstd(int fd) noex override {
	    int		rs = SR_BUGCHECK ;
	    if (m) {
		rs = (this->*m)(fd) ;
	    }
	    return rs ;
	} ; /* end method */
	sysret_t iataccess(int) noex ;
    } ; /* end struct (uatxer) */
} /* end namespace */


/* forward references */


/* local variables */

constexpr int		amask = (R_OK | W_OK | X_OK |F_OK) ;


/* exported variables */


/* exported subroutines */

int u_ataccess(int fd,cchar *fn,int am,int fl) noex {
	int		rs = SR_FAULT ;
	if (fn) ylikely {
	    rs = SR_INVALID ;
	    if (fn[0]) ylikely {
		if ((am & (~ amask)) == 0) ylikely {
		    uatxer fo(&uatxer::iataccess,fn,am,fl) ;
		    fo.fdfl.fatcwd = true ;
		    rs = fo(fd) ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (u_ataccess) */

int u_atperm(int fd,cchar *fn,int am) noex {
    	cint fl = AT_EACCESS ;
    	return u_ataccess(fd,fn,am,fl) ;
} /* end subroutine (u_atperm) */


/* local subroutines */

sysret_t uatxer::iataccess(int fd) noex {
    	int		rs ;
	if ((rs = faccessat(fd,fn,am,fl)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
} /* end method (uatxer::iataccess) */



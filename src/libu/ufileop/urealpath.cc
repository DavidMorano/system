/* urealpath SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* resolve a path without symbolic or relative components */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	u_realpath

	Description:
	This subroutine takes an existing path and creates a new path
	that does not contain either symbolic or relative components.

	Synopsis:
	int u_realpath(cchar *fname,char *rbuf,char **rpp) noex

	Arguments:
	fname		file-name to resolve
	rbuf		result buffer pointer (must be MAXPATHLEN in length)
	rpp		result pointer pointer

	Returns:
	>=0		length of resuling resolved file-name
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>		/* standard says this is necessary */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |realpath(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<localmisc.h>

#include	"ufileopbase.hh"
#include	"urealpath.h"


/* local defines */


/* imported namespaces */

using libu::ufileopbase ;		/* type */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct urealer ;
    typedef int (urealer::*urealer_m)(cchar *) noex ;
    struct urealer : ufileopbase {
	char		*rbuf ;
	char		**rpp ;
	urealer_m	m = nullptr ;
	urealer() noex { } ;
	urealer(char *b,char **r) noex : rbuf(b), rpp(r) { } ;
	int callstd(cchar *fn) noex override {
	    int		rs = SR_BUGCHECK ;
	    if (m) {
		rs = (this->*m)(fn) ;
	    }
	    return rs ;
	} ;
	sysret_t i_realpath(cchar *) noex ;
    } ; /* end struct (urealer) */
} /* end namespace */


/* forward references */

local sysret_t std_realpath(cchar *fn,char *rbuf,char **rpp) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (rbuf) {
	    rs = SR_OK ;
	    if (char *rp ; (rp = realpath(fn,rbuf)) != np) ylikely {
	        if (rpp) *rpp = rp ;
	    } else {
	        rs = (- errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (std_realpath) */


/* local variables */


/* exported variables */


/* external subroutines */

int u_realpath(cchar *fname,char *rbuf,char **rpp) noex {
    	urealer fo(rbuf,rpp) ;
	fo.m = &urealer::i_realpath ;
	return fo(fname) ;
} /* end subroutine (u_realpath) */


/* local subroutines */

sysret_t urealer::i_realpath(cchar *fn) noex {
	return std_realpath(fn,rbuf,rpp) ;
} /* end method (urealer::u_realpath) */



/* ufiletimes SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	u_filetime
	u_filetimes

	Description:
	This system call (|utime(2)|) is the older obsolete form
	of the newer |utimes(2)| call.  On some (newer) operating
	systems (Apple Darwin, or others), this call is emulated
	with the newer |utimes(2)| call.

	Notes:
	1. The fields of the UTIMBUF object are:
		actime 		- access time in seconds
		modtime 	- modification time in seconds
	2. In the Apple-Darwin operating system, this system-call,
	it is supported (both its include file and the call itself)
	but as a library call rather than a system-call.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/time.h>		/* |utimes(2)| */
#include	<utime.h>		/* |utime(2)| */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<localmisc.h>

#include	"ufileopbase.hh"
#include	"ufiletimes.h"


/* local defines */


/* local namespaces */

using libu::ufileopbase ;		/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct ufiletimer ;
    typedef int (ufiletimer::*ufiletimer_m)(cchar *) noex ;
    struct ufiletimer : ufileopbase {
	CUTIMBUF *utp ;
	CTIMEVAL *tvp ;
	ufiletimer_m	m = nullptr ;
	ufiletimer() noex { } ;
	ufiletimer(CTIMEVAL *v) noex : tvp(v) { } ;
	ufiletimer(CUTIMBUF *u) noex : utp(u) { } ;
	int callstd(cchar *fn) noex override {
	    int		rs = SR_BUGCHECK ;
	    if (m) {
		rs = (this->*m)(fn) ;
	    }
	    return rs ;
	} ;
	sysret_t i_filetime(cchar *) noex ;
	sysret_t i_filetimes(cchar *) noex ;
    } ; /* end struct (ufiletimer) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int u_filetime(cchar *fname,CUTIMBUF *utp) noex {
    	ufiletimer fo(utp) ;
	fo.m = &ufiletimer::i_filetime ;
	return fo(fname) ;
} /* end subroutine (u_filetime) */

int u_filetimes(cchar *fname,CTIMEVAL *tvp) noex {
    	ufiletimer fo(tvp) ;
	fo.m = &ufiletimer::i_filetimes ;
	return fo(fname) ;
} /* end subroutine (u_filetimes) */


/* local subroutines */

sysret_t ufiletimer::i_filetime(cchar *fname) noex {
	int		rs = SR_FAULT ;
	if (utp) {
	    if ((rs = utime(fname,utp)) < 0) {
		rs = (- errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end method (ufiletimer::i_filetime) */

sysret_t ufiletimer::i_filetimes(cchar *fname) noex {
	int		rs = SR_FAULT ;
	if (tvp) {
	    if ((rs = utimes(fname,tvp)) < 0) {
		rs = (- errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end method (ufiletimer::i_filetimes) */



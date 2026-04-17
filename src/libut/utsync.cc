/* utsync SUPPORT (X/Open Transport Interface) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® XTI subroutine */
/* XTI sync */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usupport.h>
#include	<localmisc.h>

#include	"utbase.hh"
#include	"uxti.h"


/* local defines */


/* local namespaces */

using libut::utbase ;


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct utbase_sync : utbase {
	utbase_sync() noex { } ;
	int callstd(int) noex override final ;
    } ; /* end struct (utbase_sync) */
} /* end namespace */


/* forward references */

local sysret_t	std_tsync(int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ut_sync(int fd) noex {
    	utbase_sync uo ;
	return uo(fd) ;
}


/* local subroutines */

int utbase_sync::callstd(int fd) noex {
	return std_tsync(fd) ;
}

local sysret_t std_tsync(int fd) noex {
    	int		rs ;
	if ((rs = xti_sync(fd)) < 0) {
	    switch (t_errno) {
	    case TBADF:
	        rs = SR_NOTSOCK ;
	        break ;
	    case TSYSERR:
	        rs = (- errno) ;
	        break ;
	    default:
		rs = SR_NOANODE ;
		break ;
	    } /* end switch */
	} /* end if */
	return rs ;
} /* end subroutine (std_tsync) */



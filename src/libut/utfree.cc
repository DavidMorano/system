/* utfree SUPPORT (X/Open Transport Interface) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® XTI subroutine */
/* XTI free */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
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
    struct utbase_free : utbase {
	void	*p ;
	int	stype ;
	utbase_free(void *µp,int µstype) noex {
	    p = µp ;
	    stype = µstype ;
	} ;
	int callstd(int) noex override final ;
    } ; /* end struct (utbase_free) */
} /* end namespace */


/* forward references */

local sysret_t	std_tfree(void *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ut_free(void *p,int stype) noex {
    	int		rs = SR_FAULT ;
	if (p) {
    	    utbase_free uo(p,stype) ;
	    rs = uo(0) ;
	}
	return rs ;
} /* end subroutine (ut_free) */


/* local subroutines */

int utbase_free::callstd(int) noex {
	return std_tfree(p,stype) ;
}

local sysret_t std_tfree(void *p,int stype) noex {
    	int		rs ;
	if ((rs = xti_free(p,stype)) < 0) {
	    switch (t_errno) {
	    case TSYSERR:
	        rs = (- errno) ;
		break ;
	    default:
		rs = SR_NOANODE ;
		break ;
	    } /* end switch */
	} /* end if */
	return rs ;
} /* end subroutine (std_tfree) */



/* utopen SUPPORT (X/Open Transport Interface) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® XTI subroutine */
/* XTI open */
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

namespace {
    struct utbase_open : utbase {
	cchar	*fname ;
	int	f ;
	utbase_open(cchar *fn,int µf,UTINFO *p) noex {
	    fname = fn ;
	    f = µf ;
	    utinfop = p ;
	} ;
	int callstd(int) noex override final ;
    } ; /* end struct (utbase_open) */
} /* end namespace */


/* external variables */


/* local structures */


/* forward references */

local sysret_t	std_topen(cchar *,int,UTINFO *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ut_open(cchar *fname,int f,struct t_info *ip) noex {
    	utbase_open uo(fname,f,ip) ;
	return uo(0) ;
}


/* local subroutines */

int utbase_open::callstd(int) noex {
	return std_topen(fname,f,utinfop) ;
}

local sysret_t std_topen(cchar *fname,int f,UTINFO *ip) noex {
    	int		rs ;
	if ((rs = xti_open(fname,f,ip)) < 0) {
	    switch (t_errno) {
	    case TBADFLAG:
	        rs = SR_INVALID ;
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
} /* end subroutine (std_topen) */



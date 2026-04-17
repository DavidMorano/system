/* utalloc SUPPORT (X/Open Transport Interface) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® XTI subroutine */
/* XTI allocate */
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
    struct utbase_alloc : utbase {
	int	stype ;
	int	fields ;
	void	**rpp ;
	utbase_alloc(int s,int f,void **r) noex {
	    stype = s ;
	    fields = f ;
	    rpp = r ;
	} ;
	int callstd(int) noex override final ;
    } ; /* end struct (utbase_alloc) */
} /* end namespace */


/* forward references */

local sysret_t	std_talloc(int,int,int,void **) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ut_alloc(int fd,int stype,int fields,void **rpp) noex {
    	utbase_alloc uo(stype,fields,rpp) ;
	return uo(fd) ;
}


/* local subroutines */

int utbase_alloc::callstd(int fd) noex {
	return std_talloc(fd,stype,fields,rpp) ;
}

local sysret_t std_talloc(int fd,int stype,int fields,void **pp) noex {
    	int		rs = SR_OK ;
	void *p ;
	if ((p = xti_alloc(fd,stype,fields)) == nullptr) {
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
	*pp = (rs >= 0) ? p : nullptr ;
	return rs ;
} /* end subroutine (std_talloc) */



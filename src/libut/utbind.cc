/* ut_bind SUPPORT (X/Open Transport Interface) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® XTI subroutine */
/* XTI bind */
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
#include	<uxti.h>
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
    struct utbase_bind : utbase {
	UTBIND	*retp ;
	utbase_bind(UTBIND *reqp,UTBIND *rp) noex {
	    utbindp = reqp ;
	    retp = rp ;
	} ;
	int callstd(int) noex override final ;
    } ; /* end struct (utbase_bind) */
} /* end namespace */


/* forward references */

local sysret_t	std_tbind(int,UTBIND *,UTBIND *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ut_bind(int fd,UTBIND *reqp,UTBIND *retp) noex {
    	utbase_bind uo(reqp,retp) ;
	return uo(fd) ;
}


/* local subroutines */

int utbase_bind::callstd(int fd) noex {
	return std_tbind(fd,utbindp,retp) ;
}

local sysret_t std_tbind(int fd,UTBIND *reqp,UTBIND *retp) noex {
	int		rs ;
	if ((rs = xti_bind(fd,reqp,retp)) < 0) {
	    switch (t_errno) {
	    case TBADF:
	        rs = SR_NOTSOCK ;
	        break ;
	    case TACCES:
	        rs = SR_ACCES ;
	        break ;
	    case TBADADDR:
	        rs = SR_DESTADDRREQ ;
	        break ;
	    case TBUFOVFLW:
	        rs = SR_OVERFLOW ;
	        break ;
	    case TNOADDR:
	        rs = SR_ADDRNOTAVAIL ;
	        break ;
	    case TOUTSTATE:
	        rs = SR_NOPROTOOPT ;
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
} /* end subroutine (std_tbind) */



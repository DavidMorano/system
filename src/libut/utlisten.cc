/* utlisten SUPPORT (X/Open Transport Interface) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® XTI subroutine */
/* XTI listen */
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
    struct utbase_listen : utbase {
	utbase_listen(UTCALL *callp) noex {
	    utcallp = callp ;
	} ;
	int callstd(int) noex override final ;
    } ; /* end struct (utbase_listen) */
} /* end namespace */


/* forward references */

local sysret_t	std_tlisten(int,UTCALL *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ut_listen(int fd,UTCALL *callp) noex {
    	utbase_listen uo(callp) ;
	return uo(fd) ;
}


/* local subroutines */

int utbase_listen::callstd(int fd) noex {
	return std_tlisten(fd,utcallp) ;
}

local sysret_t std_tlisten(int fd,UTCALL *callp) noex {
	int		rs ;
	if ((rs = xti_listen(fd,callp)) < 0) {
	    switch (t_errno) {
	    case TBADF:
	        rs = SR_NOTSOCK ;
	        break ;
	    case TBADQLEN:
	        rs = SR_INVALID ;
	        break ;
	    case TBUFOVFLW:
	        rs = SR_OVERFLOW ;
	        break ;
	    case TLOOK:
	        rs = SR_LOOK ;
	        break ;
	    case TNODATA:
	        rs = SR_NODATA ;
	        break ;
	    case TNOTSUPPORT:
	        rs = SR_OPNOTSUPP ;
	        break ;
	    case TOUTSTATE:
	        rs = SR_NOPROTOOPT ;
	        break ;
	    case TPROTO:
	        rs = SR_PROTO ;
	        break ;
	    case TQFULL:
	        rs = SR_DQUOT;
	        break ;
	    case TACCES:
	        rs = SR_ACCES ;
	        break ;
	    case TBADADDR:
	        rs = SR_DESTADDRREQ ;
	        break ;
	    case TNOADDR:
	        rs = SR_ADDRNOTAVAIL ;
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
} /* end subroutine (std_tlisten) */



/* utbase SUPPORT (X/Open Transport Interface) */
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

	Names:
	utbase

	Description:
	UNIX® File-Descriptor system calls.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<usupport.h>
#include	<errtimer.hh>
#include	<localmisc.h>

#include	"utbase.hh"
#include	"uxti.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace libut {
    int utbase::operator () (int fd) noex {
	int		rs = SR_BADF ;
	if (fd >= 0) {
	    errtimer	to_closewait	= utimeout[uto_closewait] ;
	    errtimer	to_again	= utimeout[uto_again] ;
	    errtimer	to_busy		= utimeout[uto_busy] ;
	    errtimer	to_nomem	= utimeout[uto_nomem] ;
	    errtimer	to_nosr		= utimeout[uto_nosr] ;
	    errtimer	to_nobufs	= utimeout[uto_nobufs] ;
	    errtimer	to_mfile	= utimeout[uto_mfile] ;
	    errtimer	to_nfile	= utimeout[uto_nfile] ;
	    errtimer	to_nolck	= utimeout[uto_nolck] ;
	    errtimer	to_nospc	= utimeout[uto_nospc] ;
	    errtimer	to_dquot	= utimeout[uto_dquot] ;
	    errtimer	to_io		= utimeout[uto_io] ;
	    reterr	r ;
	    repeat {
	        if ((rs = callstd(fd)) < 0) {
		    r(rs) ;			/* <- default causes exit */
                    switch (rs) {
                    case SR_AGAIN:
                        r = to_again(rs) ;
                        break ;
                    case SR_BUSY:
                        r = to_busy(rs) ;
                        break ;
                    case SR_NOMEM:
                        r = to_nomem(rs) ;
                        break ;
	            case SR_NOSR:
                        r = to_nosr(rs) ;
		        break ;
	            case SR_NOBUFS:
	                r = to_nobufs(rs) ;
	                break ;
                    case SR_MFILE:
                        r = to_mfile(rs) ;
                        break ;
                    case SR_NFILE:
                        r = to_nfile(rs) ;
                        break ;
	            case SR_NOLCK:
                        r = to_nolck(rs) ;
		        break ;
                    case SR_NOSPC:
                        r = to_nospc(rs) ;
		        break ;
	            case SR_DQUOT:
                        r = to_dquot(rs) ;
		        break ;
	            case SR_IO:
                        r = to_io(rs) ;
		        break ;
		    case SR_INPROGRESS: /* who thought up this? */
                        r = to_closewait(rs) ;
		        break ;
                    case SR_INTR:
			r(false) ;
                        break ;
                    } /* end switch */
		    rs = r ;
                } /* end if (callstd) */
	    } until ((rs >= 0) || r.fexit) ;
	} /* end if (valid) */
	return rs ;
    } /* end method (utbase::operator) */
} /* end namespace (libut) */



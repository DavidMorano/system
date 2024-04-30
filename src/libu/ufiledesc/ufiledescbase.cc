/* ufiledescbase SUPPORT (UNIX® file operations) */
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

	Description:
	UNIX® File-Descriptor system calls.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<climits>		/* |INT_MAX| */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdint>		/* |intptr_t| */
#include	<cstring>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<usupport.h>
#include	<clanguage.h>
#include	<errtimer.hh>
#include	<localmisc.h>

#include	"ufiledesc.h"


/* local defines */


/* imported namespaces */

using namespace	ufiledesc ;		/* namespace */

using std::nullptr_t ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int ufiledescbase::operator () (int fd) noex {
	int		rs = SR_BADF ;
	if (fd >= 0) {
	    int		to_closewait	= utimeout[uto_closewait] ;
	    errtimer	to_again	= utimeout[uto_again] ;
	    errtimer	to_nomem	= utimeout[uto_nomem] ;
	    errtimer	to_nosr		= utimeout[uto_nosr] ;
	    errtimer	to_nobufs	= utimeout[uto_nobufs] ;
	    errtimer	to_mfile	= utimeout[uto_mfile] ;
	    errtimer	to_nfile	= utimeout[uto_nfile] ;
	    errtimer	to_nolck	= utimeout[uto_nolck] ;
	    errtimer	to_nospc	= utimeout[uto_nospc] ;
	    reterr	r ;
	    repeat {
	        if ((rs = callstd(fd)) < 0) {
		    r(rs) ;			/* <- default causes exit */
                    switch (rs) {
                    case SR_AGAIN:
                        r = to_again(rs) ;
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
		    case SR_INPROGRESS: /* who thought up this? */
		        if (f.fclose) {
		            rs = msleep(to_closewait * 1000) ;
		        }
		        break ;
                    case SR_INTR:
		        if (! f.fintr) {
			    r(false) ;
		        }
                        break ;
                    } /* end switch */
		    rs = r ;
                } /* end if (error) */
	    } until ((rs >= 0) || r.fexit) ;
	} /* end if (valid) */
	return rs ;
}
/* end method (ufiledescbase::operator) */



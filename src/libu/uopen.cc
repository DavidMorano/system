/* uopen SUPPORT */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	No, we are not the same as UNIX® all of the time!  We return
	PFNOSUPPORT if a protocol-family (PF) is not supported!

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<sys/stat.h>
#include	<sys/wait.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<cerrno>
#include	<usystem.h>
#include	<usupport.h>
#include	<localmisc.h>

#include	"uopen.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* local variables */


/* exported subroutines */

int u_socket(int pf,int pt,int proto) noex {
	int		rs = SR_INVALID ;
	if ((pf >= 0) && (pt >= 0) && (proto >= 0)) {
	    int		to_nomem = utimeout[uto_nomem] ;
	    int		to_nosr = utimeout[uto_nosr] ;
	    bool	f_exit = false ;
	    repeat {
	        if ((rs = socket(pf,pt,proto)) < 0) rs = (- errno) ;
	        if (rs < 0) {
	            switch (rs) {
	            case SR_NOMEM:
	                if (to_nomem-- > 0) {
			    msleep(1000) ;
		        } else {
			    f_exit = true ;
		        }
	                break ;
	            case SR_NOSR:
	                if (to_nosr-- > 0) {
			    msleep(1000) ;
		        } else {
			    f_exit = true ;
		        }
	                break ;
	            case SR_INTR:
	                break ;
		    default:
		        f_exit = true ;
		        break ;
	            } /* end switch */
	        } /* end if */
	    } until ((rs >= 0) || f_exit) ;
	    if (rs == SR_AFNOSUPPORT) rs = SR_PFNOSUPPORT ;
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (u_socket) */



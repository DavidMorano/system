/* u_sysinfo SUPPORT */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/systeminfo.h>
#include	<cerrno>
#include	<climits>		/* |INT_MAX| */
#include	<vsystem.h>
#include	<localmisc.h>


/* local defines */


/* external variables */


/* exported subroutines */

int u_sysinfo(char *ubuf,int ulen,int req) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (ubuf) {
	    rs = SR_OVERFLOW ;
	    if (ulen >= 1) {
		rs = SR_INVALID ;
		if (req >= 0) {
		    clong	llen = (ulen+1) ;
		    long	result ;
		    rs = SR_OK ;
	            if ((result = sysinfo(req,ubuf,llen)) < 0) {
		        rs = (- errno) ;
	            }
	            if (result > 0) {
		        len = (int) ((result-1) & INT_MAX) ;
	            }
	            if ((rs >= 0) && (result > llen)) {
		        rs = SR_OVERFLOW ;
	            }
	        } /* end if (valid) */
	    } /* end if (not-overflow) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (u_sysinfo) */



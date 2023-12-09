/* u_sysinfo */
/* lang=C++23 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>
#include	<limits.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* local structures */


/* external variables */


/* exported subroutines */

#if	defined(OSNAME_SunOS) && (OSNAME_SunOS > 0)

#include	<sys/systeminfo.h>

int u_sysauxinfo(char *ubuf,int ulen,int req) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (ubuf) {
	    rs = SR_INVALID ;
	    if ((ulen > 0) && (req >= 0)) {
	        long	result ;
	        long	llen = (ulen+1) ;
	        if ((result = sysinfo(request,ubuf,llen)) < 0) rs = (- errno) ;
	        if (result > 0) len = (int) ((result-1) & INT_MAX) ;
	        if ((rs >= 0) && (result > llen)) rs = SR_OVERFLOW ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (u_sysauxinfo) */

#else /* defined(OSNAME_SunOS) && (OSNAME_SunOS > 0) */

int u_sysauxinfo(char *ubuf,int ulen,int req) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (ubuf) {
	    rs = SR_INVALID ;
	    if ((ulen > 0) && (req >= 0)) {
		rs = SR_NOSYS ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (u_sysauxinfo) */

#endif /* defined(OSNAME_SunOS) && (OSNAME_SunOS > 0) */



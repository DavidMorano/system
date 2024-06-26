/* usys_sunos SUPPORT */
/* lang=C++20 */

/* define various sytem (global) variables */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David D-A- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

	We define some SunOS-specific subroutines.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */

/* USYS_SUNOS start */
#if	defined(OSNAME_SunOS) && (OSNAME_SunOS > 0)


#include	<sys/systeminfo.h>
#include	<cerrno>
#include	<climits>		/* |INT_MAX| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<intsat.h>

#include	"usys.h"


static sysret_t sunos_sysinfo(char *ubuf,int ulen,int req) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (ubuf) {
	    rs = SR_INVALID ;
	    if (req >= 0) {
	        rs = SR_OVERFLOW ;
	        if (ulen >= 1) {
		    csize	usz = (ulen+1) ;
	            if (long res ; (res = sysinfo(req,ubuf,usz)) >= 0) {
			if (res > usz) {
			    rs = SR_OVERFLOW ;
			} else {
			    rs = intsat(res) ;
			    len = rs ;
			}
		    } else {
		        rs = (- errno) ;
	            }
	        } /* end if (not-overflow) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (sunos_sysinfo) */

sysret_t ugetnisdom(char *rbuf,int rlen) noex {
	cint	req = SI_SRPC_DOMAIN ;	/* <- whew! nothing is easy */
	return sunos_sysinfo(rbuf,rlen,req) ;
}
/* end subroutine (sunos_ugetnisdom) */


/*----------------------------------------------------------------------------*/
/* LOADAVGINT begin */
#if	defined(SYSHAS_LOADAVGINT) && (SYSHAS_LOADAVGINT > 0)

unixret_t kloadavg(int *la,int n) noex {
	return __getloadavg(la,n) ;
}

#endif /* defined(SYSHAS_LOADAVGINT) && (SYSHAS_LOADAVGINT > 0) */
/* LOADAVGINT end */
/*----------------------------------------------------------------------------*/


#endif /* defined(OSNAME_SunOS) && (OSNAME_SunOS > 0) */
/* USYS_SUNOS finish */



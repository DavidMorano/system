/* uctimeconvx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX� library-3c */
/* time-conversion functions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A�D� Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	uc_localtime
	uc_gmtime
	uc_ztime
	uc_mktime

	Description:
	Time conversion subroutines.

	Synopsis:
	int uc_localtime(custime *dt,TM *tmp) noex
	int uc_gmtime(custime *dt,TM *tmp) noex
	int uc_ztime(custime *tp,TM *tsp,int z) noex
	int uc_mktime(TM *tmp,time_t *rp) noex

	Arguments:
	dt	time to convert
	tmp	pointer to TM object
	z	??
	rp	result-pointer
	
	Returns:
	>=0	OK
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cerrno>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |memcpy(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<aflag.hh>
#include	<localmisc.h>

#include	"uctimeconv.h"


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

int uc_localtime(custime *tp,TM *tsp) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (tp && tsp) ylikely {
	    rs = SR_OK ;
	    errno = 0 ;
	    if (syshas.localtimer) {
	        if (TM *rp ; (rp = localtime_r(tp,tsp)) == np) {
	            rs = (- errno) ;
		}
	    } else {
		rs = SR_NOSYS ;
	    } /* end if_constexpr (f_reentrant) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_localtime) */

int uc_gmtime(custime *tp,TM *tsp) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (tp && tsp) ylikely {
	    rs = SR_OK ;
	    errno = 0 ;
	    if (syshas.gmtimer) {
	        if (TM *rp ; (rp = gmtime_r(tp,tsp)) == np) {
	            rs = (- errno) ;
		}
	    } else {
		rs = SR_NOSYS ;
	    } /* end if_constexpr (f_reentrant) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_gmtime) */

int uc_ztime(custime *tp,TM *tsp,int z) noex {
	int		rs ;
	if (z) {
	    rs = uc_localtime(tp,tsp) ;
	} else {
	    rs = uc_gmtime(tp,tsp) ;
	}
	return rs ;
}
/* end subroutine (uc_ztime) */

int uc_mktime(TM *tmp,time_t *rp) noex {
	int		rs = SR_FAULT ;
	if (tmp && rp) ylikely {
	    time_t	res = 0 ; /* used-multiple */
	    rs = SR_OK ;
	    errno = 0 ;
	    if ((res = mktime(tmp)) < 0) {
	        if (errno) rs = (- errno) ;
	    }
	    if (rp) {
	        *rp = (rs >= 0) ? res : 0 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_mktime) */



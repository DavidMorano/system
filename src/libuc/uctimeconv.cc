/* uctimeconv SUPPORT */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* time-conversion functions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	uc_localtime
	uc_gmtime

	Description:
	Time conversion subroutines.

	Synopsis:
	int uc_localtime(const time_t *dt,TM *tmp) noex
	int uc_gmtime(const time_t *dt,TM *tmp) noex

	Arguments:
	dt	time to convert
	tmp	pointer to TM object
	
	Returns:
	>=0	OK
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cerrno>
#include	<ctime>
#include	<cstring>		/* <- for |memcpy(3c)| */
#include	<usystem.h>
#include	<aflag.hh>
#include	<localmisc.h>


/* local defines */

#if	defined(SYSHAS_LOCALTIMER) && SYSHAS_LOCALTIMER
#define	F_REENTRANT	1
#else
#define	F_REENTRANT	0
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static aflag		uctimeconvmx ;

constexpr bool		f_reentrant = F_REENTRANT ;


/* exported variables */


/* exported subroutines */

int uc_localtime(const time_t *tp,TM *tsp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (tp && tsp) {
	    TM		*rp ;
	    rs = SR_OK ;
	    errno = 0 ;
	    if_constexpr (f_reentrant) {
	        if ((rp = localtime_r(tp,tsp)) == nullptr) {
	            rs = (- errno) ;
		}
	    } else {
	        if ((rs = uc_forklockbegin(-1)) >= 0) {
	            if ((rs = uctimeconvmx.lockbegin) >= 0) {
	                if ((rp = localtime(tp)) == nullptr) {
	                    rs = (- errno) ;
		        } else {
	                    memcpy(tsp,rp,sizeof(TM)) ;
		        }
		        rs1 = uctimeconvmx.lockend ;
		        if (rs >= 0) rs = rs1 ;
		    } /* end if (uctimeconvlock) */
	            rs1 = uc_forklockend() ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (fork-lock) */
	    } /* end if_constexpr (f_reentrant) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_localtime) */

int uc_gmtime(const time_t *tp,TM *tsp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (tp && tsp) {
	    TM		*rp ;
	    rs = SR_OK ;
	    errno = 0 ;
	    if_constexpr (f_reentrant) {
	        if ((rp = gmtime_r(tp,tsp)) == nullptr) {
	            rs = (- errno) ;
		}
	    } else {
	        if ((rs = uc_forklockbegin(-1)) >= 0) {
	            if ((rs = uctimeconvmx.lockbegin) >= 0) {
	                if ((rp = gmtime(tp)) == nullptr) {
	                    rs = (- errno) ;
		        } else {
	                    memcpy(tsp,rp,sizeof(TM)) ;
		        }
		        rs1 = uctimeconvmx.lockend ;
		        if (rs >= 0) rs = rs1 ;
		    } /* end if (uctimeconvlock) */
	            rs1 = uc_forklockend() ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (fork-lock) */
	    } /* end if_constexpr (f_reentrant) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_gmtime) */



/* uctimeconv SUPPORT */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
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
	int uc_localtime(const time_t *,TM *) noex
	int uc_gmtime(const time_t *,TM *) noex

	Returns:
	>=0	OK
	<-	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cerrno>
#include	<ctime>
#include	<cstring>		/* <- for |memcpy(3c)| */
#include	<usystem.h>
#include	<usupport.h>
#include	<localmisc.h>


/* local defines */

#if	defined(SYSHAS_LOCALTIMER) && SYSHAS_LOCALTIMER
#define	F_REENTRANT	1
#else
#define	F_REENTRANT	0
#endif


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr bool		f_reentrant = F_REENTRANT ;


/* exported subroutines */

int uc_localtime(const time_t *tp,TM *tsp) noex {
	int		rs = SR_FAULT ;
	if (tp && tsp) {
	    TM		*rp ;
	    errno = 0 ;
	    if constexpr (f_reentrant) {
	        if ((rp = localtime_r(tp,tsp)) == nullptr) {
	            rs = (- errno) ;
		}
	    } else {
	        if ((rp = localtime(tp)) == nullptr) {
	            rs = (- errno) ;
		}
	        if (rs >= 0) {
	            memcpy(tsp,rp,sizeof(TM)) ;
		}
	    } /* end if-constexpr */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_localtime) */

int uc_gmtime(const time_t *tp,TM *tsp) noex {
	int		rs = SR_FAULT ;
	if (tp && tsp) {
	    TM		*rp ;
	    errno = 0 ;
	    if constexpr (f_reentrant) {
	        if ((rp = gmtime_r(tp,tsp)) == nullptr) {
	            rs = (- errno) ;
		}
	    } else {
	        if ((rp = gmtime(tp)) == nullptr) {
	            rs = (- errno) ;
		}
	        if (rs >= 0) {
	            memcpy(tsp,rp,sizeof(TM)) ;
		}
	    } /* end if-constexpr */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_gmtime) */



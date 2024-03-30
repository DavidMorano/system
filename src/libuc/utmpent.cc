/* utmpent SUPPORT */
/* lang=C++20 */

/* methods for the utmpent object */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-07-19, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

        This code provides the methods for the UTMPENT object.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<strn.h>
#include	<mkchar.h>
#include	<localmisc.h>
#include	<syshasutmpx.h>

#include	"utmpent.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* forward references */


/* local variables */

constexpr bool		f_utmpsession = SYSHASUTMP_SESSION ;
constexpr bool		f_utmpsyslen = SYSHASUTMP_SYSLEN ;
constexpr bool		f_utmpexit = SYSHASUTMP_EXIT ;


/* exported variables */


/* exported subroutines */

int utmpent_start(utmpent *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = memclear(op) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (utmpent_start) */

int utmpent_finish(utmpent *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = memclear(op) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (utmpent_finish) */

int utmpent_settype(utmpent *op,int type) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->ut_type = type ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (utmpent_settype) */

int utmpent_setpid(utmpent *op,pid_t sid) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->ut_pid = sid ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (utmpent_setpid) */

int utmpent_setsession(utmpent *op,int sess) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if constexpr (f_utmpsession) {
	        op->ut_session = sess ;
	    } else {
		(void) sess ;
	    } /* end if-constexpr (f_utmpsession) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (utmpent_setsession) */

int utmpent_setid(utmpent *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = (strnwcpy(op->ut_id,UTMPENT_LID,sp,sl) - op->ut_id) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (utmpent_setid) */

int utmpent_setline(utmpent *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = (strnwcpy(op->ut_line,UTMPENT_LLINE,sp,sl) - op->ut_line) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (utmpent_setline) */

int utmpent_setuser(utmpent *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = (strnwcpy(op->ut_user,UTMPENT_LUSER,sp,sl) - op->ut_user) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (utmpent_setuser) */

int utmpent_sethost(utmpent *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = (strnwcpy(op->ut_host,UTMPENT_LHOST,sp,sl) - op->ut_host) ;
	    if constexpr (f_utmpsyslen) {
	        op->ut_syslen = rs ;
	    } /* end if-constexpr (f_utmpsyslen) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (utmpent_sethost) */

int utmpent_gettype(utmpent *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = mkchar(op->ut_type) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (utmpent_gettype) */

int utmpent_getpid(utmpent *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->ut_pid ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (utmpent_getpid) */

int utmpent_getsession(utmpent *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if constexpr (f_utmpsession) {
	        rs = op->ut_session ;
	    } else {
	        rs = SR_OK ;
	    } /* end if-constexpr (f_utmpsession) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (utmpent_getsession) */

int utmpent_getid(utmpent *op,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = strnlen(op->ut_id,UTMPENT_LID) ;
	    if (rpp) *rpp = op->ut_id ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (utmpent_getid) */

int utmpent_getline(utmpent *op,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = strnlen(op->ut_line,UTMPENT_LLINE) ;
	    if (rpp) *rpp = op->ut_line ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (utmpent_getline) */

int utmpent_getuser(utmpent *op,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = strnlen(op->ut_user,UTMPENT_LUSER) ;
	    if (rpp) *rpp = op->ut_user ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (utmpent_getuser) */

int utmpent_gethost(utmpent *op,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = strnlen(op->ut_host,UTMPENT_LHOST) ;
	    if constexpr (f_utmpsyslen) {
	        if (op->ut_syslen > 0) {
	            if (rs > op->ut_syslen) {
		        rs = op->ut_syslen ;
		    }
	        }
	    } /* end if-constexpr (f_utmpsession) */
	    if (rpp) *rpp = op->ut_host ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (utmpent_gethost) */



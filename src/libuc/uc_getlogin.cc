/* ucgetlogin SUPPORT */
/* lang=C++20 */

/* get the username of this login session */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is just the wrapper for the standard system |getlogin(3c)|,
	but it is reentrant where available.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>		/* |getlogin(3c)| */
#include	<cerrno>
#include	<climits>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<sncpyx.h>


/* local defines */

#ifdef	COMMENT /* good try - but this is not the real limit */
#define	BUFLEN		_POSIX_LOGIN_NAME_MAX
#endif

#ifndef	SYSHAS_GETLOGINR
#define	SYSHAS_GETLOGINR	0
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local variables */

constexpr bool		f_getloginr = SYSHAS_GETLOGINR ;


/* exported variables */


/* exported subroutines */

int uc_getlogin(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    if ((rs = getbufsize(getbufsize_un)) >= 0) {
		cint	ulen = rs ;
		rs = SR_OVERFLOW ;
	        if ((rlen >= 0) && (rlen < ulen)) {
	            if_constexpr (f_getloginr) {
	                if ((rs = getlogin_r(rbuf,rlen)) != 0) rs = (- errno) ;
	                if (rs >= 0) rs = strlen(rbuf) ;
	            } else {
	                cchar	*rp = getlogin() ;
	                rs = (rp != nullptr) ? 0 : (- errno) ;
	                if (rs >= 0) rs = sncpy1(rbuf,rlen,rp) ;
	            } /* end if_constexpr (f_getloginr) */
	        } /* end if (valid) */
	    } /* end if (getbufsize) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_getlogin) */



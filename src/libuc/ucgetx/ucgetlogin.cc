/* ucgetlogin SUPPORT */
/* charset=ISO8859-1 */
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
#include	<cerrno>		/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<bufsizeget.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ucgetx.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

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
	if (rbuf) ylikely {
	    if ((rs = bufsizeget(bufsize_un)) >= 0) ylikely {
		cint	ulen = rs ;
		rs = SR_OVERFLOW ;
	        if ((rlen >= 0) && (rlen < ulen)) ylikely {
	            if_constexpr (f_getloginr) {
	                if ((rs = getlogin_r(rbuf,rlen)) != 0) {
			    rs = (neg errno) ;
			} /* end if (error) */
	                if (rs >= 0) {
			    rs = lenstr(rbuf) ;
			}
	            } else {
	                cchar	*rp = getlogin() ;
	                rs = (rp != nullptr) ? 0 : (neg errno) ;
	                if (rs >= 0) rs = sncpy1(rbuf,rlen,rp) ;
	            } /* end if_constexpr (f_getloginr) */
	        } /* end if (valid) */
	    } /* end if (bufsizeget) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_getlogin) */



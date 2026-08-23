/* ucsockjoin SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* join a UNIX®-Socket-Datagram */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_sockjoin

  	Description:
	This subroutine joins a UNIX® socket of the datagram variety.

	Synopsis:
	int uc_sockjoin(int fd,SOCKADDR *sap,int sal,int of,mode_t om) noex

	Arguments:
	fd		file-descriptor (presumably a socket)
	sap		socket-address-pointer
	sal		socket-address-length
	of		open-flags
	om		open-mode

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/socket.h>		/* POSIX® |AF_{xx}| */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® |O_{xx}| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<sockaddress.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ucdescsock.h"

/* local defines */


/* importæd namespaces */

using libuc::libmem ;			/* variable */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int binder	(int,SOCKADDR *,int,int,mode_t) noex ;
local int setperm	(SOCKADDR *,mode_t) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_sockjoin(int fd,SOCKADDR *sap,int sal,int of,mode_t om) noex {
    	int		rs = SR_FAULT ;
	if (sap) ylikely {
	    rs = SR_BADF ;
	    if (fd >= 0) ylikely {
		rs = SR_INVALID ;
		if (of >= 0) ylikely {
	            if (of & O_CREAT) {
			rs = binder(fd,sap,sal,of,om) ;
	            } else {
	                rs = u_connect(fd,sap,sal) ;
	            }
	        } /* end if (valid) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_sockjoin) */


/* local subroutines */

local int binder(int fd,SOCKADDR *sap,int sal,int of,mode_t om) noex {
	int		rs ;
	if ((rs = u_bind(fd,sap,sal)) >= 0) ylikely {
            if (of & O_MINMODE) {
	        sockaddress	*sa = (sockaddress *) sap ;
	        if (cint af = AF_UNIX ; (rs = sa->getaf) == af) {
	            rs = setperm(sap,om) ;
	        } /* end if (UNIX® address family) */
	    } /* end if (min-mod wanted) */
	} /* end if (u_bind) */
	return rs ;
} /* end subroutine (binder) */

local int setperm(SOCKADDR *sap,mode_t om) noex {
	int		rs ;
	int		rs1 ;
	if (char *sbuf ; (rs = lm_mp(&sbuf)) >= 0) ylikely {
            sockaddress	*sa = (sockaddress *) sap ;
	    cint	slen = rs ;
            if ((rs = sa->getaddr(sbuf,slen)) >= 0) ylikely {
                rs = u_minmod(sbuf,om) ;
                if (rs < 0) {
                    u_unlink(sbuf) ;
                }
            } /* end if (sockaddress_getaddr) */
	    rs1 = lm_free(sbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
        return rs ;
} /* end subroutine (setperm) */



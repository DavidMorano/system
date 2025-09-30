/* uc_sockjoin */
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

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<sockaddress.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_sockjoin(int fd,SOCKADDR *sap,int sal,int of,mode_t om) noex {
	int		rs = SR_BADF ;
	cint		f_create = ((of&O_CREAT)?1:0) ;
	if (fd >= 0) {
	    if (f_create) {
	        if ((rs = u_bind(fd,sap,sal)) >= 0) {
		    SOCKADDRESS	*sa = (SOCKADDRESS *) sap ;
		    cint	af = AF_UNIX ;
		    if ((rs = sockaddress_getaf(sa)) == af) {
		        cint	slen = MAXPATHLEN ;
		        char	sbuf[MAXPATHLEN+1] ;
		        if ((rs = sockaddress_getaddr(sa,sbuf,slen)) >= 0) {
		            rs = u_chmod(sbuf,om) ;
		            if (rs < 0) {
			        uc_unlink(sbuf) ;
			    }
		        } /* end if (sockaddress_getaddr) */
		    } /* end if (UNIX® address family) */
	        } /* end if (u_bind) */
	    } else {
	        rs = u_connect(fd,sap,sal) ;
	    }
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (uc_sockjoin) */



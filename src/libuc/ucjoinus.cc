/* ucjoinus SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* join a UNIX®-Socket-Datagram */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_joinus

	Description:
	This subroutine joins a UNIX® socket of the datagram variety.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<sockaddress.h>
#include	<mkchar.h>
#include	<localmisc.h>


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int ucjoiner(int,SOCKADDR *,int,int,mode_t) noex ;
static int ucbinder(int,SOCKADDR *,int,int,mode_t) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_joinus(int fd,SOCKADDR *sap,int sal,int of,mode_t om) noex {
	int		rs = SR_FAULT ;
	if (sap) {
	    rs = SR_BADF ;
	    if (fd >= 0) {
		rs = SR_INVALID ;
		if (of >= 0) {
		    rs = ucjoiner(fd,sap,sal,of,om) ;
	        } /* end if (valid) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_joinus) */


/* local subroutines */

static int ucjoiner(int fd,SOCKADDR *sap,int sal,int of,mode_t om) noex {
	int		rs ;
	if (of & O_CREAT) {
	    rs = ucbinder(fd,sap,sal,of,om) ;
	} else {
	    rs = uc_connect(fd,sap,sal) ;
	}
	return rs ;
}
/* end subroutine (ucjoiner) */

static int ucbinder(int fd,SOCKADDR *sap,int sal,int of,mode_t om) noex {
	int		rs ;
	int		rs1 ;
        if ((rs = u_bind(fd,sap,sal)) >= 0) {
            if (of & O_MINMODE) {
                sockaddress	*sa = (sockaddress *) sap ;
                cint		af = AF_UNIX ;
                if ((rs = sockaddress_getaf(sa)) == af) {
                    if (char *sbuf ; (rs = malloc_mp(&sbuf)) >= 0) {
			cint	slen = rs ;
                        if ((rs = sockaddress_getaddr(sa,sbuf,slen)) >= 0) {
                            rs = u_chmod(sbuf,om) ;
                            if (rs < 0) {
                                uc_unlink(sbuf) ;
                            }
                        } /* end if (sockaddress_getaddr) */
			rs1 = malloc_free(sbuf) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
                } /* end if (UNIX®Â® address family) */
            } /* end if (min-mod) */
        } /* end if (u_bind) */
	return rs ;
}
/* end subroutine (ucbinder) */



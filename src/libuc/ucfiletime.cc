/* ucfiletime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_utime
	uc_utimes

	Description:
	This is the middleware version of |u_utime(3u)|.

	Synopsis:
	int uc_utime(cchar *fname,CUTIMBUF *utp) noex
	int uc_utimes(cchar *fname,CTIMEVAL *utp) noex
	This is the middleware version of |u_utime(3u)|.

	Arguments:
	fname		file-name
	utp		result object pointer

	Returns:
	>=0		OK
	<0		error code (system-return)

	Notes:
	1. The fields of the UTIMBUF object are:
		actime 		- access time in seconds
		modtime 	- modification time in seconds

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/time.h>		/* for |timeval| */
#include	<utime.h>		/* for |utimbuf| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<libmallocxx.h>
#include	<mkx.h>
#include	<localmisc.h>


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local defines */


/* exported variables */


/* exported subroutines */

int uc_utime(cchar *fname,CUTIMBUF *utp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (fname && utp) ylikely {
	    if (char *ebuf ; (rs = libmalloc_mp(&ebuf)) >= 0) ylikely {
	        if ((rs = mkpathexp(ebuf,fname,-1)) > 0) {
		    rs = u_utime(ebuf,utp) ;
	        } else if (rs == 0) {
		    rs = u_utime(fname,utp) ;
	        }
	        rs1 = uc_libfree(ebuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_utime) */

int uc_utimes(cchar *fname,CTIMEVAL *utp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (fname && utp) ylikely {
	    if (char *ebuf ; (rs = libmalloc_mp(&ebuf)) >= 0) ylikely {
	        if ((rs = mkpathexp(ebuf,fname,-1)) > 0) {
		    rs = u_utimes(ebuf,utp) ;
	        } else if (rs == 0) {
		    rs = u_utimes(fname,utp) ;
	        }
	        rs1 = uc_libfree(ebuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_utimes) */



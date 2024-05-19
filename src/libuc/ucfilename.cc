/* ucfilename SUPPORT */
/* lang=C++20 */

/* translation layer interface for UNIX� equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_utime

	Description:
	This is the middleware version of |u_utime(3u)|.

	Returns:
	>=0		OK
	<0		error code (system-return)

	Notes:
	1. The fields of the UTIMBUF object are:
		actime 		- access time in seconds
		modtime 	- modification time in seconds

	Name:
	uc_utimes

	Descrption:
	This is the middleware version of |u_utime(3u)|.

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/time.h>		/* for |timeval| */
#include	<utime.h>		/* for |utimbuf| */
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
	if (fname && utp) {
	    char	*ebuf{} ;
	    if ((rs = libmalloc_mp(&ebuf)) >= 0) {
	        if ((rs = mkexpandpath(ebuf,fname,-1)) > 0) {
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
	if (fname && utp) {
	    char	*ebuf{} ;
	    if ((rs = libmalloc_mp(&ebuf)) >= 0) {
	        if ((rs = mkexpandpath(ebuf,fname,-1)) > 0) {
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



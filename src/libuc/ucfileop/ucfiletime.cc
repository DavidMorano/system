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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<prefixfn.h>
#include	<localmisc.h>


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_utime(cchar *fname,CUTIMBUF *utp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if (fname && utp) ylikely {
	    cchar	*fn ;
	    if (prefixfn pf ; (rs = pf.start(fname,-1,&fn)) > 0) ylikely {
		{
		    rs = u_utime(fn,utp) ;
		    rv = rs ;
	        }
	        rs1 = pf.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (uc_utime) */

int uc_utimes(cchar *fname,CTIMEVAL *utp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if (fname && utp) ylikely {
	    cchar	*fn ;
	    if (prefixfn pf ; (rs = pf.start(fname,-1,&fn)) > 0) ylikely {
		{
		    rs = u_utimes(fn,utp) ;
		    rv = rs ;
	        }
	        rs1 = pf.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (uc_utimes) */



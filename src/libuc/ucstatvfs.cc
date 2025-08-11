/* ucstatvfs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* get status on a filesystem */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/statvfs.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<usystem.h>
#include	<libmallocxx.h>
#include	<mkx.h>			/* |mkpathexp(3uc)| */
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_statvfs(cchar *fname,ustatvfs *sbp) noex {
	int		rs = SR_FAULT ;
	if (fname && sbp) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		if (char *ebuf ; (rs = libmalloc_mp(&ebuf)) >= 0) {
	            if ((rs = mkpathexp(ebuf,fname,-1)) >= 0) {
	                if (rs > 0) fname = ebuf ;
	                rs = u_statvfs(fname,sbp) ;
	            } /* end if (mkpathexp) */
		    rs = rslibfree(rs,ebuf) ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_statvfs) */



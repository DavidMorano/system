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
#include	<sys/types.h>
#include	<sys/statvfs.h>
#include	<sys/stat.h>
#include	<sys/uio.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<usystem.h>
#include	<libmallocxx.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */

extern "C" {
    extern int	mkvarpath(char *,cchar *,int) noex ;
    extern int	hasvarpathprefix(cchar *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_statvfs(cchar *fname,USTATVFS *sbp) noex {
	int		rs = SR_FAULT ;
	if (fname && sbp) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		if (char *efname ; (rs = libmalloc_mp(&efname)) >= 0) {
	            if ((rs = mkvarpath(efname,fname,-1)) >= 0) {
	                if (rs > 0) fname = efname ;
	                rs = u_statvfs(fname,sbp) ;
	            } /* end if (mkvarpath) */
		    rs = rslibfree(rs,efname) ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_statvfs) */



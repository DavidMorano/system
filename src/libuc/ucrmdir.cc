/* ucrmdir SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* like |mkdir(2)| */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<localmisc.h>


/* local defines */


/* imported namespaces */


/* local typedefs */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	mkexpandpath(char *,cchar *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_rmdir(cchar *fname) noex {
	int		rs = SR_FAULT ;
	if (fname) ylikely {
	    rs = SR_INVALID ;
	    if (fname[0]) ylikely {
	        if ((char *ebuf ; (rs = libmalloc_mp(&ebuf)) >= 0) ylikely {
	            if ((rs = mkexpandpath(ebuf,fname,-1)) > 0) {
		        rs = u_rmdir(ebuf) ;
	            } else if (rs == 0) {
		        rs = u_rmdir(fname) ;
	            }
	            rs = rslibfree(rs,ebuf) ;
	        } /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_rmdir) */



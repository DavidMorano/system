/* uccreate SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* get status on a file */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<fcntl.h>		/* |mode_t| */
#include	<usystem.h>
#include	<libmallocxx.h>
#include	<mkx.h>
#include	<localmisc.h>

#include	"ucopen.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_create(cchar *fname,mode_t om) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = 0 ; /* return-value */
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		if (char *ebuf ; (rs = libmalloc_mp(&ebuf)) >= 0) {
	            if ((rs = mkpathexp(ebuf,fname,-1)) > 0) {
		        rs = u_creat(ebuf,om) ;
			fd = rs ;
	            } else if (rs == 0) {
		        rs = u_creat(fname,om) ;
			fd = rs ;
	            }
	            rs1 = uc_libfree(ebuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
		if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (uc_create) */



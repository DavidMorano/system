/* btruncate SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* "Basic I-O" package similiar to "stdio" */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We truncate a BFILE stream.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int btruncate(bfile *op,off_t off) noex {
	int		rs ;
	if ((rs = bfile_magic(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (off >= 0) {
	        if ((rs = bfile_flush(op)) >= 0) {
		    csize	soff = size_t(off) ;
		    if (soff < op->offset) {
	                rs = uc_ftruncate(op->fd,off) ;
		    }
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (btruncate) */



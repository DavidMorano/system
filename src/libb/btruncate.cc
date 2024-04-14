/* btruncate SUPPORT */
/* lang=C++20 */

/* "Basic I-O" package similiar to "stdio" */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

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


/* exported variables */


/* exported subroutines */

int btruncate(bfile *fp,off_t off) noex {
	int		rs = SR_OK ;

	if (fp == NULL) return SR_FAULT ;

	if (fp->magic != BFILE_MAGIC) return SR_NOTOPEN ;

	if (! fp->f.nullfile) {
	    if ((rs = bfile_flush(fp)) >= 0) {
	        rs = uc_ftruncate(fp->fd,off) ;
	    }
	}

	return rs ;
}
/* end subroutine (btruncate) */



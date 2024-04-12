/* btell SUPPORT */
/* lang=C++20 */

/* "Basic I/O" package (BIO) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We tell where we are (in a BFILE stream).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>
#include	<unistd.h>
#include	<fcntl.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */


/* external subroutines */


/* external variables */


/* exported subroutines */

int btell(bfile *fp,off_t *rp) noex {
	off_t	telloff = 0 ;
	int		rs = SR_OK ;

	if (fp == NULL) return SR_FAULT ;

	if (fp->magic != BFILE_MAGIC) return SR_NOTOPEN ;

	if (! fp->f.nullfile) {
	    telloff = fp->offset ;
	    if (! fp->f.notseek) rs = SR_NOTSEEK ;
	}

	if (rp != NULL) *rp = telloff ;
	rs = (telloff & UINT_MAX) ;
	return rs ;
}
/* end subroutine (btell) */



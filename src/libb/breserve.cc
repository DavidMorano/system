/* breserve SUPPORT */
/* lang=C++20 */

/* "Basic I/O" package */
/* last modifed %G% version %I% */


/* reivsion history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Some kind of reserve function.

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


/* exported subroutines */

int breserve(bfile *fp,int n) noex {
	int		rs = SR_OK ;

	if (fp == NULL) return SR_FAULT ;

	if (fp->magic != BFILE_MAGIC) return SR_NOTOPEN ;

	if (n < 0) return SR_INVALID ;

	if (! fp->f.nullfile) {
	    if (fp->f.write) {
	        int	blenr = (fp->bdata + fp->bsize - fp->bp) ;
	        if (n > blenr) rs = bfile_flushn(fp,-1) ;
	    }
	}

	return rs ;
}
/* end subroutine (breserve) */



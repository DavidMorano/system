/* bflush SUPPORT */
/* lang=C++20 */

/* "Basic I/O" package */
/* last modifed %G% version %I% */


/* reivsion history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Flush interface for BFILE.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */


/* external subroutines */

extern "C" {
    extern int	bfile_flushn(bfile *,int) noex ;
}


/* external variables */


/* exported subroutines */

int bflushn(bfile *fp,int n) noex {
	int		rs = SR_FAULT ;
	if (fp) {
	    rs = SR_NOTOPEN ;
	    if (fp->magic == BFILE_MAGIC) {
		rs = SR_OK ;
	        if (! fp->f.nullfile) {
	            if (fp->f.write && (fp->len > 0)) {
	                rs = bfile_flushn(fp,n) ;
		    }
	        }
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bflushn) */

int bflush(bfile *fp) noex {
	return bflushn(fp,-1) ;
}
/* end subroutine (bflush) */



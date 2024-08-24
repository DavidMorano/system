/* breserve SUPPORT */
/* lang=C++20 */

/* "Basic I-O" package */
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

int breserve(bfile *op,int n) noex {
	int		rs ;
	if ((rs = bfile_magic(op)) > 0) {
	    rs = SR_INVALID ;
	    if (n >= 0) {
		rs = SR_OK ;
	        if (op->f.writing) {
	            cint	blenr = (op->bdata + op->bsize - op->bp) ;
	            if (n > blenr) {
			rs = bfile_flushn(op,-1) ;
		    }
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (breserve) */



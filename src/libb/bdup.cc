/* bdup SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* "Basic I-O" package */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	bdup

	Description:
	Duplicate a BFILE stream.  Take the existing BFILE stream and
	duplicate it to the passwd BFILE object (by pointer).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int bdup(bfile *op,bfile *fnewp) noex {
	int		rs ;
	if ((rs = bfile_magic(op,fnewp)) > 0) {
	    bfile	*bnewp = fnewp ;
	    memcpy(bnewp,op) ; /* shallow-copy */
	    if ((rs = bfile_flush(op)) >= 0) {
	        if ((rs = u_dup(op->fd)) >= 0) {
	            fnewp->fd = rs ;
	            if (op->bsize > 0) {
			cint	bsz = op->bsize ;
		        if (void *vp ; (rs = uc_malloc(bsz,&vp)) >= 0) {
	                    fnewp->bdata = (char *) vp ;
	                    fnewp->bbp = (char *) vp ;
	                    fnewp->bp = (char *) vp ;
	                }
	            }
		    if (rs < 0) {
		        uc_close(fnewp->fd) ;
		        fnewp->fd = -1 ;
		    }
	        } /* end if (u_dup) */
	        if (rs < 0) {
		    fnewp->magic = 0 ;
	        }
	    } /* end if (bfile_flush) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (bdup) */



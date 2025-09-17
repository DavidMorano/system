/* breserve SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* "Basic I-O" package */
/* last modifed %G% version %I% */


/* reivsion history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	breserve

	Description:
	Some kind of reserve function.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
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
	        if (op->fl.writing) {
	            cint blenr = intconv(op->bdata + op->bsize - op->bp) ;
	            if (n > blenr) {
			rs = bfile_flushn(op,-1) ;
		    }
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (breserve) */



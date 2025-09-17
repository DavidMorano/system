/* bflush SUPPORT */
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

	Flush interface for BFILE.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<usystem.h>

#include	"bfile.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int bflushn(bfile *op,int n) noex {
	int		rs ;
	if ((rs = bfile_magic(op)) > 0) {
	    if (op->fl.writing && (op->len > 0)) {
		rs = bfile_flushn(op,n) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (bflushn) */

int bflush(bfile *op) noex {
	return bflushn(op,-1) ;
}
/* end subroutine (bflush) */



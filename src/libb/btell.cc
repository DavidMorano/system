/* btell SUPPORT */
/* lang=C++20 */

/* "Basic I-O" package (BIO) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

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
#include	<intsat.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int btell(bfile *op,off_t *rp) noex {
	int		rs ;
	if ((rs = bfile_magic(op)) > 0) {
	    coff	ro = op->offset ;
	    if (rp) *rp = ro ;
	    rs = intsat(ro) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (btell) */



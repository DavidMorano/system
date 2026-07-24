/* btell SUPPORT */
/* charset=ISO8859-1 */
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
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<intsat.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"bfile.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int btell(bfile *op,off_t *rp) noex {
	int		rs ;
	if ((rs = bfile_magic(op)) > 0) ylikely {
	    coff	ro = op->offset ;
	    if (rp) *rp = ro ;
	    {
		csize foff = size_t(ro) ;
	        rs = intsat(foff) ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (btell) */



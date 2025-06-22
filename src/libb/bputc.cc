/* bputc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* "Basic I-O" package similiar to "stdio" */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Print out a single character (within the BFILE framework).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<usystem.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int bputc(bfile *op,int ch) noex {
	int		rs ;
	int		wlen = 0 ;
	char		wbuf[2] = { char(ch) } ;
	if ((rs = bwrite(op,wbuf,1)) > 0) {
	    wlen = rs ;
	    if ((ch == '\n') && (op->bm == bfilebm_line)) {
	        rs = bfile_flush(op) ;
	    }
	} /* end if (bwrite) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bputc) */



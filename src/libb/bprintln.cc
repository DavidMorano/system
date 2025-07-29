/* bprintln SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* print out a single line */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-02-09, David A­D­ Morano
	This code was originally written (due to laziness?).

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	bprintln

	Description:
	This subroutine is very simple.  It just avoids calling
	|bprintf(3b)| when that would have been completely fine!
	:-)

	Synopsis:
	int bprintline(bfile *op,cchar *lbuf,int llen) noex

	Arguments:
	op		file-pointer
	lbuf		buffer of characters to print out
	llen		length of characters to print

	Returns:
	>=0		number of characters printed
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstdlib>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"bfile.h"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int bprintln(bfile *op,cchar *lbuf,int llen) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = bfile_magic(op,lbuf)) > 0) {
	    if ((rs = bfile_ckwr(op)) >= 0) {
	        bool	feol = false ;
	        if (llen < 0) llen = lenstr(lbuf) ;
	        feol = feol || (llen == 0) ;
	        feol = feol || (lbuf[llen-1] != CH_NL) ;
	        if (feol) {
		    rs = breserve(op,(llen+1)) ;
	        }
	        if ((rs >= 0) && (llen > 0)) {
	            rs = bwrite(op,lbuf,llen) ;
	            wlen += rs ;
	        }
	        if ((rs >= 0) && feol) {
	            rs = bputc(op,CH_NL) ;
	            wlen += rs ;
	        }
	    } /* end if (writing) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bprintln) */



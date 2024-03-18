/* bprintln SUPPORT */
/* lang=C++20 */

/* print out a single line */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-02-09, David A�D� Morano
	This subroutine was originally written (due to laziness?).

*/

/* Copyright � 2004 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	bprintln

	Description:
	This subroutine is very simple.  It just avoids calling
	|bprintf(3b)| when that would have been completely fine!
	:-)

	Synopsis:
	int bprintline(bfile *fp,cchar *lbuf,int llen) noex

	Arguments:
	fp		file-pointer
	lbuf		buffer of characters to print out
	llen		length of characters to print

	Returns:
	>=0		number of characters printed
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */


/* exported variables */


/* exported subroutines */

int bprintln(bfile *fp,cchar *lbuf,int llen) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = bmagic(fp,lbuf)) >= 0) {
	    if ((rs = bfile_active(fp)) > 0) {
	        bool	feol = false ;
	        if (llen < 0) llen = strlen(lbuf) ;
	        feol = feol || (llen == 0) ;
		feol = feol || (lbuf[llen-1] != CH_NL) ;
	        if (feol) {
		    rs = breserve(fp,(llen+1)) ;
		}
	        if ((rs >= 0) && (llen > 0)) {
	            rs = bwrite(fp,lbuf,llen) ;
	            wlen += rs ;
	        }
	        if ((rs >= 0) && feol) {
	            rs = bputc(fp,CH_NL) ;
	            wlen += rs ;
	        }
	    } /* end if (active) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bprintln) */

int bprint(bfile *fp,cchar *lbuf,int llen) noex {
	return bprintln(fp,lbuf,llen) ;
}
/* end subroutine (bprint) */

int bprintline(bfile *fp,cchar *lbuf,int llen) noex {
	return bprintln(fp,lbuf,llen) ;
}
/* end subroutine (bprintline) */



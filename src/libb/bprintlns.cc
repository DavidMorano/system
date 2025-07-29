/* bprintlns SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* print out lines but filled to the margin */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-02-09, David A­D­ Morano
        I snarfed this from another program of mine.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	bprintlns

	Description:
	This subroutine prints out some lines but not to exceed the
	line-folding length (specified by the caller).

	Synopsis:
	int bprintlns(bfile *op,int flen,cchar *sp,int sl) noex

	Arguments:
	op		file pointer (BIO)
	flen		line folding length (maximum line length)
	sp		buffer to print out
	sl		amount of data in buffer to print out

	Returns:
	>=0		amount of data printed out
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<estrings.h>
#include	<linefold.h>
#include	<localmisc.h>		/* |COLUMNS| + |NTABCOLS| */

#include	"bfile.h"

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;

/* local defines */

#define	INDENT		2		/* indent 2 columns */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		ind = INDENT ;


/* exported variables */


/* exported subroutines */

int bprintlns(bfile *op,int flen,cchar *lbuf,int llen) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = bfile_magic(op,lbuf)) > 0) {
	    if ((rs = bfile_ckwr(op)) >= 0) {
	        if (flen < 2) flen = COLUMNS ;
		if (linefold lf ; (rs = lf.start(flen,ind,lbuf,llen)) >= 0) {
		    int		ll ;
		    cchar	*lp ;
		    for (int i = 0 ; (ll = lf.getln(i,&lp)) >= 0 ; i += 1) {
			rs = bprintln(op,lp,ll) ;
			wlen += rs ;
			if (rs < 0) break ;
		    } /* end for */
		    rs1 = lf.finish ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (linefold) */
	    } /* end if (writing) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bprintlns) */



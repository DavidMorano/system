/* bwriteblanks SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* write blanks */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	bwriteblanks
	bwritechrs

	Description:
	This subroutine just prints out some blank (or other) characters.

	Synopsis:
	int bwriteblanks(bfile *op,int n) noex
	int bwritechars(bfile *op,int ch,int n) noex

	Arguments:
	op		BFILE object pointer
	ch		character to write
	n		number of characters to write

	Returns:
	>=0		number of characters written
	<0		error-code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<strw.h>		/* |strwset(3uc)| */
#include	<localmisc.h>

#include	"bfile.h"

import libutil ;

/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	bwritebuf(bfile *,cchar *,int) noex ;


/* local variables */

constexpr char		blanks[] = "        " ;

constexpr int		nblanks = cstrlen(blanks) ;


/* exported variables */


/* exported subroutines */

int bwriteblanks(bfile *op,int n) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = bfile_magic(op)) > 0) {
	    rs = SR_INVALID ;
	    if (n >= 0) {
		rs = bwritebuf(op,blanks,n) ;
		wlen = rs ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bwriteblanks) */

int bwritechrs(bfile *op,int ch,int n) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = bfile_magic(op)) > 0) {
	    rs = SR_INVALID ;
	    if (n >= 0) {
		if (ch != ' ') {
		    char	wbuf[nblanks+1] ;
		    strwset(wbuf,ch,nblanks) ;
		    rs = bwritebuf(op,wbuf,n) ;
		    wlen = rs ;
		} else {
		    rs = bwritebuf(op,blanks,n) ;
		    wlen = rs ;
		}
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bwritechrs) */


/* local subroutines */

static int bwritebuf(bfile *op,cchar *wbuf,int n) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	int		blen = n ;
	while ((rs >= 0) && (blen > 0)) {
	    cint	mlen = min(blen,nblanks) ;
	    rs = bwrite(op,wbuf,mlen) ;
	    wlen += rs ;
	    blen -= rs ;
	} /* end while */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bwritebuf) */



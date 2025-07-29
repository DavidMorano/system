/* bprintcleanlns SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* print a clean (cleaned up) line of text */
/* version %I% last-modified %G% */

#define	CF_LINEFOLD	1		/* use |linefold(3dam)| */

/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	bprintcleanlns

	Description:
	This subroutine prints out a cleaned up line of text.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<linefold.h>
#include	<rmx.h>
#include	<ischarx.h>
#include	<localmisc.h>		/* |COLUMNS| */

#include	"bfile.h"

import libutil ;

/* local defines */

#ifndef	CF_LINEFOLD
#define	CF_LINEFOLD	1		/* use |linefold(3dam)| */
#endif


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	bprintfold(bfile *,int,cchar *,int) noex ;
static int	bprintcleanliner(bfile *,int,cchar *,int) noex ;


/* local variables */

const bool	f_linefold = CF_LINEFOLD ;


/* exported variables */


/* exported subroutines */

int bprintcleanlns(bfile *op,int linelen,cchar *lp,int ll) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = bfile_magic(op,lp)) > 0) {
	    if ((rs = bfile_ckwr(op)) >= 0) {
	        if (linelen <= 0) linelen = COLUMNS ;
	        if (ll < 0) ll = cstrlen(lp) ;
	        if_constexpr (f_linefold) {
		    rs = bprintfold(op,linelen,lp,ll) ;
		    wlen += rs ;
	        } else {
	            rs = bprintcleanliner(op,linelen,lp,ll) ;
	            wlen += rs ;
	        } /* end if_constexpr (f_linefold) */
	    } /* end if (writig) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bprintcleanlines) */


/* local subroutines */

static int bprintfold(bfile *op,int linelen,cchar *lp,int ll) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if (linefold lf ; (rs = lf.start(linelen,0,lp,ll)) >= 0) {
	    int		sl ;
	    cchar	*sp ;
	    for (int i = 0 ; (sl = lf.getln(i,&sp)) >= 0 ; i += 1) {
		rs = bprintcleanliner(op,linelen,sp,sl) ;
		wlen += rs ;
		if (rs < 0) break ;
	    } /* end for */
	    rs1 = lf.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (linefold) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bprintfold) */

static int bprintcleanliner(bfile *op,int linelen,cchar *lp,int ll) noex {
	int		rs = SR_OK ;
	int		rl = rmeol(lp,ll) ;
	int		wlen = 0 ;
	while ((rs >= 0) && (rl > 0)) {
	    cint	ml = min(rl,linelen) ;
	    rs = bprintcleanln(op,lp,ml) ;
	    wlen += rs ;
	    lp += ml ;
	    rl -= ml ;
	} /* end while */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bprintcleanliner) */



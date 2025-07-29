/* bprintcleanln SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* print a clean (cleaned up) line of text */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	bprintcleanln

	Description:
	This subroutine prints out a cleaned up line of text.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<linebuffer.h>
#include	<rmx.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"bfile.h"
#include	"snclean.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int bprintcleanln(bfile *op,cchar *lp,int ll) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = bfile_magic(op,lp)) > 0) {
	    cint	rl = rmeol(lp,ll) ;
	    if (linebuffer lo ; (rs = lo.start) >= 0) {
		if ((rs = snclean(lo.lbuf,lo.llen,lp,rl)) >= 0) {
	            rs = bprintln(op,lo.lbuf,rs) ;
		    wlen = rs ;
		} /* end if (snclean) */
		rs1 = lo.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (linebuffer) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bprintcleanln) */



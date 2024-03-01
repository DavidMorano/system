/* snflags SUPPORT */
/* lang=C++20 */

/* make string version of some flags */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Ths object is used in the creation of flags strings.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>
#include	<usystem.h>		/* <- for |memclear(3u)| */
#include	<usupport.h>		/* |memclear(3u)| */
#include	<storebuf.h>
#include	<localmisc.h>

#include	"snflags.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* foward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int snflags_start(snflags *op,char *bp,int bl) noex {
	int		rs = SR_FAULT ;
	if (op && bp) {
	    rs = memclear(op) ;
	    op->bp = bp ;
	    op->bl = bl ;
	    *bp = '\0' ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (snflags_start) */

int snflags_addstr(snflags *op,cchar *sp) noex {
	return snflags_addstrw(op,sp,-1) ;
}

int snflags_addstrw(snflags *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (op && sp) {
	    rs = SR_OK ;
	    if ((sl > 0) || sp[0]) {
	        if (op->c++ > 0) {
	            cint	ch_comma = ',' ;
	            rs = storebuf_char(op->bp,op->bl,op->bi,ch_comma) ;
	            op->bi += rs ;
		    rl += rs ;
	        }
	        if (rs >= 0) {
	            rs = storebuf_strw(op->bp,op->bl,op->bi,sp,sl) ;
	            op->bi += rs ;
		    rl += rs ;
	        }
	    } /* end if (non-empty) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (snflags_addstrw) */

int snflags_finish(snflags *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->bi ;
	    op->bp = nullptr ;
	    op->bl = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (snflags_finish) */



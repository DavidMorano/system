/* outstore SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* manage some dynamic output storage (mostly for SHIO use) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	outstore

	Description:
	This object manages a small buffer for output storage
	purposes.  This is mostly (or perhaps only) used as a helper
	object for the SHIO object when it is:

	1. in SFIO mode
	2. the output is a terminal

	Othersize, SHIO just outputs data using the regular calls.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"outstore.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* exported variables */


/* exported subroutines */

int outstore_start(outstore *op) noex {
    	OUTSTORE	*hop = op ;
	int		rs = SR_FAULT ;
	if (op) {
	    rs = memclear(hop) ;
	}
	return rs ;
}
/* end subroutine (outstore_start) */

int outstore_finish(outstore *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->dbuf) {
	        op->dbuf[0] = '\0' ; /* cute safety trick */
	        rs1 = uc_free(op->dbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbuf = nullptr ;
	    }
	    op->sbuf[0] = '\0' ;
	    op->len = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (outstore_finish) */

int outstore_get(outstore *op,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->len ;
	    if (rpp) {
	        *rpp = (op->dbuf) ? op->dbuf : op->sbuf ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (outstore_get) */

int outstore_clear(outstore *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->len = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (outstore_clear) */

int outstore_strw(outstore *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op && sp) {
	    cint	slen = OUTSTORE_SLEN ;
	    int		rlen ;
	    if (sl < 0) sl = strlen(sp) ;
	    if (op->dbuf) {
	        rlen = (op->dlen-op->len) ;
	        if (sl > rlen) {
		    cint	dlen = (op->dlen+sl+slen) ;
		    char	*dp{} ;
		    if ((rs = uc_realloc(op->dbuf,(dlen+1),&dp)) >= 0) {
		        char	*rp = (dp+op->len) ;
		        op->dbuf = dp ;
		        op->dlen = dlen ;
		        rs = (strwcpy(rp,sp,sl) - rp) ;
		        op->len += rs ;
		    }
	        } else {
		    char	*rp = (op->dbuf+op->len) ;
		    rs = (strwcpy(rp,sp,sl) - rp) ;
		    op->len += rs ;
	        }
	    } else {
	        rlen = (slen-op->len) ;
	        if (sl > rlen) {
		    cint	dlen = max((sl+slen),(2*slen)) ;
		    if (char *dp{} ; (rs = uc_malloc((dlen+1),&dp)) >= 0) {
		        op->dlen = dlen ;
		        op->dbuf = dp ;
		        dp = strwcpy(dp,op->sbuf,op->len) ;
		        rs = (strwcpy(dp,sp,sl) - dp) ;
		        op->len += rs ;
		    } /* end if (memory-allocation) */
	        } else {
		    char	*rp = (op->sbuf+op->len) ;
		    rs = (strwcpy(rp,sp,sl) - rp) ;
		    op->len += rs ;
	        }
	    } /* end if (current mode) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (outstore_strw) */



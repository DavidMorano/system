/* numincr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* number incrementing operations */
/* version %I% last-modified %G% */


/* revision history:

	- 1998-12-01, David A­D­ Morano
	This module was originally written for hardware CAD support.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	numincr

	Description:
	This object procides a "number incrementing" operation.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<cfdec.h>
#include	<cfa26.h>
#include	<ctdec.h>
#include	<ctdecp.h>
#include	<cta26.h>
#include	<ischarx.h>		/* |isupperlatin(3uc)| */
#include	<localmisc.h>

#include	"numincr.h"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int numincr_start(numincr *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    memclear(op) ;
	    op->prec = 1 ;
	    rs = numincr_load(op,sp,sl) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (numincr_start) */

int numincr_finish(numincr *op) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (numincr_finish) */

int numincr_load(numincr *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op && sp) {
	    int		ch ;
	    if (sl < 0) sl= lenstr(sp) ;
	    if (op->prec < sl) {
	        op->prec = sl ;
	    }
	    ch = sp[0] ;
	    if (isdigitlatin(ch)) {
	        if (int v ; (rs = cfdeci(sp,sl,&v)) >= 0) {
	            op->v = v ;
		}
	    } else if (isalphalatin(ch)) {
	        op->fl.alpha = true ;
	        op->fl.uc = isupperlatin(ch) ;
	        if (int v ; (rs = cfa26(sp,sl,&v)) >= 0) {
	            op->v = v ;
	        }
	    } else {
	        rs = SR_INVALID ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (numincr_load) */

int numincr_setprec(numincr *op,int prec) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (prec < 1) prec = 1 ;
	    if (prec > NUMINCR_MAXPREC) {
	        prec = NUMINCR_MAXPREC ;
	    }
	    if (prec > op->prec) {
	        op->prec = prec ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (numincr_setprec) */

int numincr_incr(numincr *op,int incr) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->v += incr ;
	}
	return rs ;
}
/* end subroutine (numincr_incr) */

int numincr_cvtstr(numincr *op,char *rbuf,int rlen,int prec) noex {
	int		rs = SR_FAULT ;
	(void) prec ;
	if (op && rbuf) {
	    if (op->fl.alpha) {
	        cint	type = (op->fl.uc) ? 'A' : 'a' ;
	        rs = cta26(rbuf,rlen,type,op->prec,op->v) ;
	    } else {
	        rs = ctdecp(rbuf,rlen,op->prec,op->v) ;
	    } /* end if */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (numincr_cvtstr) */



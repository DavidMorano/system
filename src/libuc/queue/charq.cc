/* charq SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* character queue module */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This, or something almost identical to this, is actually
	very old.  This code might date to about 1983.  But I
	just cleaned it up to the present form below.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	charq

	Description:
	This module implements a character queue object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<localmisc.h>

#include	"charq.h"


/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int charq_start(charq *op,int sz) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_INVALID ;
	    if (sz > 1) {
		if (void *vp ; (rs = libmem.mall(sz,&vp)) >= 0) {
		    op->qbuf = charp(vp) ;
	            op->qlen = sz ;
	            op->count = 0 ;
	            op->ri = 0 ;
	            op->wi = 0 ;
	        } /* end if (memory-allocation) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (charq_start) */

int charq_finish(charq *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->qbuf) {
	        rs1 = libmem.free(op->qbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        op->qbuf = nullptr ;
	    }
	    op->qlen = 0 ;
	    op->count = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (charq_finish) */

int charq_ins(charq *op,int ch) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OVERFLOW ;
	    if (op->count < op->qlen) {
	        op->qbuf[op->wi] = charconv(ch) ;
	        op->wi = ((op->wi + 1) % op->qlen) ;
	        op->count += 1 ;
	        rs = op->count ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (charq_ins) */

int charq_rem(charq *op,char *cp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_EMPTY ;
	    if (op->count > 0) {
	        if (cp) *cp = op->qbuf[op->ri] ;
	        op->ri = ((op->ri + 1) % op->qlen) ;
	        op->count -= 1 ;
	        rs = op->count ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (charq_rem) */

int charq_remall(charq *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->ri = 0 ;
	    op->wi = 0 ;
	    op->count = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (charq_remall) */

int charq_size(charq *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->qlen ;
	}
	return rs ;
}
/* end subroutine (charq_size) */

int charq_count(charq *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->count ;
	}
	return rs ;
}
/* end subroutine (charq_count) */



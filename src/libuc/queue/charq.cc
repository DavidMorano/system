/* charq SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* character queue module */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This obejct module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	charq

	Description:
	This module implements a character queue object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<usystem.h>

#include	"charq.h"


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
		if (void *vp ; (rs = uc_libmalloc(sz,&vp)) >= 0) {
		    op->buf = charp(vp) ;
	            op->sz = sz ;
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
	    if (op->buf) {
	        rs1 = uc_libfree(op->buf) ;
	        if (rs >= 0) rs = rs1 ;
	        op->buf = nullptr ;
	    }
	    op->sz = 0 ;
	    op->count = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (charq_finish) */

int charq_ins(charq *op,int ch) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OVERFLOW ;
	    if (op->count < op->sz) {
	        op->buf[op->wi] = charconv(ch) ;
	        op->wi = ((op->wi + 1) % op->sz) ;
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
	        if (cp) *cp = op->buf[op->ri] ;
	        op->ri = ((op->ri + 1) % op->sz) ;
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
	    rs = op->sz ;
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



/* charq */
/* lang=C++20 */

/* character queue module */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This obejct module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module implements a character queue object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"charq.h"


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int charq_start(charq *op,int size) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_INVALID ;
	    if (size > 1) {
		void	*vp{} ;
	        if ((rs = uc_libmalloc(size,&vp)) >= 0) {
		    op->buf = (char *) vp ;
	            op->size = size ;
	            op->count = 0 ;
	            op->ri = 0 ;
	            op->wi = 0 ;
	        }
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
	    op->size = 0 ;
	    op->count = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (charq_finish) */

int charq_ins(charq *op,int ch) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OVERFLOW ;
	    if (op->count < op->size) {
	        op->buf[op->wi] = ch ;
	        op->wi = ((op->wi + 1) % op->size) ;
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
	        op->ri = ((op->ri + 1) % op->size) ;
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
	    rs = op->size ;
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



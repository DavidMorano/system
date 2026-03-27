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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<ulogerror.h>
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
	if (op) ylikely {
	    rs = SR_INVALID ;
	    if (sz > 1) ylikely {
		if (void *vp ; (rs = libmem.mall(sz,&vp)) >= 0) ylikely {
		    op->qbuf = charp(vp) ;
	            op->qlen = sz ;
	            op->cnt = 0 ;
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
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->qbuf) ylikely {
	        rs1 = libmem.free(op->qbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        op->qbuf = nullptr ;
	    }
	    op->qlen = 0 ;
	    op->cnt = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (charq_finish) */

int charq_ins(charq *op,int ch) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OVERFLOW ;
	    if (op->cnt < op->qlen) ylikely {
	        op->qbuf[op->wi] = char(ch) ;
	        op->wi = ((op->wi + 1) % op->qlen) ;
	        op->cnt += 1 ;
	        rs = op->cnt ;
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (charq_ins) */

int charq_rem(charq *op,char *cp) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_EMPTY ;
	    if (op->cnt > 0) ylikely {
	        if (cp) *cp = op->qbuf[op->ri] ;
	        op->ri = ((op->ri + 1) % op->qlen) ;
	        op->cnt -= 1 ;
	        rs = op->cnt ;
	    } /* end if (not-empty) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (charq_rem) */

int charq_remall(charq *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    op->ri = 0 ;
	    op->wi = 0 ;
	    op->cnt = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (charq_remall) */

int charq_size(charq *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = op->qlen ;
	}
	return rs ;
}
/* end subroutine (charq_size) */

int charq_count(charq *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = op->cnt ;
	}
	return rs ;
}
/* end subroutine (charq_count) */

int charq::ins(int ch) noex {
	return charq_ins(this,ch) ;
}

int charq::rem(char *rp) noex {
	return charq_rem(this,rp) ;
}

void charq::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("charq",rs,"fini-finish") ;
	}
} /* end method (charq::dtor) */

charq::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (qbuf) ylikely {
	    rs = cnt ;
	} /* end if (non-null) */
	return rs ;
} /* end method (charq::operator) */

int charq_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case charqmem_start:
	        rs = charq_start(op,a) ;
	        break ;
	    case charqmem_remall:
	        rs = charq_remall(op) ;
	        break ;
	    case charqmem_size:
	        rs = charq_size(op) ;
	        break ;
	    case charqmem_count:
	        rs = charq_count(op) ;
	        break ;
	    case charqmem_len:
	        rs = charq_count(op) ;
	        break ;
	    case charqmem_finish:
	        rs = charq_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (charq_co::operator) */



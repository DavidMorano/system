/* sintq SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* short-integer queue module */
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
	sintq

	Description:
	This module implements a short-intger queue object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>		/* |umem| */
#include	<localmisc.h>
#include	<debprintf.h>		/* |DEBPRINTF(3uc)| */

#include	"sintq.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/debug.ccm"

import libutil ;			/* |memcopy(3u)| */
import debug ;				/* |debprintf(3uc)| */

/* local defines */


/* imported namespaces */

using libu::umem ;			/* variable */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int sinq_ext(sintq *) noex ;


/* local variables */

cint		esz = szof(short) ;	/* entry-size */


/* exported variables */


/* exported subroutines */

int sintq_start(sintq *op,int ne) noex {
	int		rs = SR_FAULT ;
	if (ne <= 0) ne = SINTQ_DEFENTS ;
	if (op) ylikely {
	    cint sz = (ne * esz) ;
	    rs = SR_INVALID ;
	    if (sz > 1) ylikely {
		if (void *vp ; (rs = umem.mall(sz,&vp)) >= 0) ylikely {
		    op->qbuf = shortp(vp) ;
	            op->qlen = ne ;
	            op->cnt = 0 ;
	            op->ri = 0 ;
	            op->wi = 0 ;
	        } /* end if (memory-allocation) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sintq_start) */

int sintq_finish(sintq *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->qbuf) ylikely {
	        rs1 = umem.free(op->qbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        op->qbuf = nullptr ;
	    } /* end if (memory-deallocation) */
	    op->qlen = 0 ;
	    op->cnt = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sintq_finish) */

int sintq_ins(sintq *op,int ch) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	DEBPRINTF("ent wi=%d\n",op->wi) ;
	if (op) ylikely {
	    cauto add = [&op] (int cha) noex -> int {
	        op->qbuf[op->wi] = short(cha) ;
	        op->wi = ((op->wi + 1) % op->qlen) ;
	        op->cnt += 1 ;
	        return op->cnt ;
	    } ; /* end lambda (add) */
	    if (op->cnt < op->qlen) ylikely {
		rs = add(ch) ;
	    } else if ((rs = sinq_ext(op)) >= 0) {
		rs = add(ch) ;
	    } /* end if */
	    c = op->cnt ;
	} /* end if (non-null) */
	DEBPRINTF("ret rs=%d c=%d ql=%d wi=%d\n",rs,op->cnt,op->qlen,op->wi) ;
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (sintq_ins) */

int sintq_rem(sintq *op,short *rp) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_EMPTY ;
	    if (op->cnt > 0) ylikely {
	        if (rp) *rp = op->qbuf[op->ri] ;
	        op->ri = ((op->ri + 1) % op->qlen) ;
	        op->cnt -= 1 ;
	        rs = op->cnt ;
	    } /* end if (not-empty) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sintq_rem) */

int sintq_remread(sintq *op,short *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ; /* return-value */
	if (op && rbuf) ylikely {
	    rs = SR_OK ;
	    if (int ml ; rlen > 0) ylikely {
	            cint len = op->cnt ;
	            ml = min(len,rlen) ;
	            for (i = 0 ; (rs >= 0) && (i < ml) ; i += 1) {
			if (short v ; (rs = sintq_rem(op,&v)) >= 0) {
			    rbuf[i] = v ;
			} else if (rs == SR_EMPTY) {
			    rs = SR_OK ;
			    break ;
			}
	            } /* end for */
	    } /* end if (non-zero positive) */
	    rbuf[i] = 0 ;
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (sintq_remread) */

int sintq_remall(sintq *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    op->ri = 0 ;
	    op->wi = 0 ;
	    op->cnt = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sintq_remall) */

int sintq_size(sintq *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = op->qlen ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sintq_size) */

int sintq_count(sintq *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = op->cnt ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sintq_count) */

int sintq::ins(int ch) noex {
	return sintq_ins(this,ch) ;
}

int sintq::rem(short *rp) noex {
	return sintq_rem(this,rp) ;
}

int sintq::remread(short *rp,int rl) noex {
	return sintq_remread(this,rp,rl) ;
}

void sintq::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("sintq",rs,"fini-finish") ;
	}
} /* end method (sintq::dtor) */

sintq::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (qbuf) ylikely {
	    rs = cnt ;
	} /* end if (non-null) */
	return rs ;
} /* end method (sintq::operator) */

int sintq_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case sintqmem_start:
	        rs = sintq_start(op,a) ;
	        break ;
	    case sintqmem_remall:
	        rs = sintq_remall(op) ;
	        break ;
	    case sintqmem_size:
	        rs = sintq_size(op) ;
	        break ;
	    case sintqmem_count:
	        rs = sintq_count(op) ;
	        break ;
	    case sintqmem_len:
	        rs = sintq_count(op) ;
	        break ;
	    case sintqmem_finish:
	        rs = sintq_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (sintq_co::operator) */

local int sinq_ext(sintq *op) noex {
	cint		nlen = (op->qlen * 2) ;
    	int		rs = SR_BUGCHECK ;
	{
	    cchar *fmt = "ent ql=%d cnt=%d ri=%d wr=%d\n" ;
	    DEBPRINTF(fmt,op->qlen,op->cnt,op->ri,op->wi) ;
	}
	if (op->cnt == op->qlen) ylikely {
	    if (short *na ; (rs = umem.mall((nlen * esz),&na)) >= 0) {
		short *dp ; /* used-muliple */
		int ni ; /* used-afterwards */
		if ((ni = (op->qlen - op->ri)) > 0) {
		    cshort *sp = (op->qbuf + op->ri) ;
		    cint sz = (ni * esz) ;
		    dp = (na + 0) ;
		    memcopy(dp,sp,sz) ;
	        } /* end if (read-index) */
		if (cint nw = (op->wi - 0) ; nw > 0) {
		    cshort *sp = (op->qbuf + 0) ;
		    cint sz = (nw * esz) ;
		    dp = (na + ni) ;
		    memcopy(dp,sp,sz) ;
		} /* end if (write-index) */
		if ((rs = umem.free(op->qbuf)) >= 0) {
		    op->ri = 0 ;
		    op->wi = op->qlen ;
		    op->qbuf = na ;
		    op->qlen = nlen ;
		} /* end if (memory-deallocation) */
	    } /* end if (umem.mall) */
	} /* end if (bug-check) */
	DEBPRINTF("ret rs=%d nlen=%d\n",rs,nlen) ;
	return rs ;
} /* end subroutine (sinrq_ext) */



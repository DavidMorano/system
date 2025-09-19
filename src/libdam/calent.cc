/* calent SUPPORT */
/* charset=ISO8859-1 */
/* lang=C20 */

/* calendar entry object (for CALYEARS) */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	calent

	Description:
	We manage an individual calendar entry.  We do not actually
	hold the entry (proper).  Rather we hold a reference to the
	entry.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<estrings.h>
#include	<sbuf.h>
#include	<sfx.h>
#include	<hash.h>		/* |hash_elf(3dam)| */
#include	<localmisc.h>

#include	"calent.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int calent_ctor(calent *op,Args ... args) noex {
    	CALENT		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (calent_ctor) */

static int calent_dtor(calent *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (calent_dtor) */

template<typename ... Args>
static inline int calent_magic(calent *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == CALENT_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (calent_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int calent_start(calent *op,calent_q *qp,uint loff,int llen) noex {
	cint		ne = CALENT_NLE ;
	int		rs ;
	if ((rs = calent_ctor(op,qp)) >= 0) {
	    cint	sz = ne * szof(calent_ln) ;
	    op->cidx = -1 ;
	    op->q = *qp ;
	    op->voff = loff ;
	    op->vlen = llen ;
	    if (calent_ln *elp ; (rs = uc_malloc(sz,&elp)) >= 0) {
	        op->lines = elp ;
	        op->e = ne ;
	        op->i += 1 ;
	        op->magic = CALENT_MAGIC ;
	        elp->loff = loff ;
	        elp->llen = llen ;
	    } /* end if (memory-allocation) */
	    if (rs < 0) {
		calent_dtor(op) ;
	    }
	} /* end if (calent_ctor) */
	return rs ;
}
/* end subroutine (calent_start) */

int calent_finish(calent *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = calent_magic(op)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (op->e > 0) {
		rs = SR_BADFMT ;
	        if ((op->i >= 0) && (op->i <= op->e)) {
		    rs = SR_OK ;
	            if (op->lines) {
	                rs1 = uc_free(op->lines) ;
	                if (rs >= 0) rs = rs1 ;
	                op->lines = nullptr ;
	            }
	            op->i = 0 ;
	            op->e = 0 ;
	        } /* end if (good) */
	    } /* end if (open) */
	    {
		rs1 = calent_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (calent_finish) */

int calent_setidx(calent *op,int cidx) noex {
	int		rs ;
	if ((rs = calent_magic(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (cidx >= 0) {
	        rs = cidx ;
	        op->cidx = cidx ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (calent_setidx) */

int calent_add(calent *op,uint loff,int llen) noex {
	int		rs ;
	if ((rs = calent_magic(op)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (op->e > 0) {
		rs = SR_BADFMT ;
	        if ((op->i >= 0) && (op->i <= op->e)) {
	            calent_ln	*elp ;
	            int		sz ;
		    rs = SR_OK ;
	            if (op->i == op->e) {
	                cint	ne = (op->e * 2) + CALENT_NLE ;
	                sz = ne * szof(calent_ln) ;
	                if ((rs = uc_realloc(op->lines,sz,&elp)) >= 0) {
	                    op->e = ne ;
	                    op->lines = elp ;
	                }
	            }
	            if (rs >= 0) {
	                op->vlen = ((loff + llen) - op->voff) ;
	                elp = (op->lines + op->i) ;
	                elp->loff = loff ;
	                elp->llen = llen ;
	                op->i += 1 ;
	            } /* end if (ok) */
	        } /* end if (good) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (calent_add) */

int calent_samecite(calent *op,CALENT *oop) noex {
	int		rs ;
	int		f = true ;
	if ((rs = calent_magic(op,oop)) >= 0) {
	    f = f && (op->q.y == oop->q.y) ;
	    f = f && (op->q.m == oop->q.m) ;
	    f = f && (op->q.d == oop->q.d) ;
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (calent_samecite) */

int calent_mkhash(calent *op,cchar *md) noex {
	int		rs ;
	if ((rs = calent_magic(op,md)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (op->e > 0) {
		rs = SR_NOTOPEN ;
	        if (op->lines) {
	            calent_ln	*elp = op->lines ;
	            rs = SR_OK ;
	            if (! op->fl.hash) {
	                uint	hash = 0 ;
	                int	sl, cl ;
	                cchar	*sp, *cp ;
	                for (int i = 0 ; i < op->i ; i += 1) {
	                    sp = (md + elp[i].loff) ;
	                    sl = elp[i].llen ;
	                    while ((cl = nextfield(sp,sl,&cp)) > 0) {
	                        hash += hash_elf(cp,cl) ;
	                        sl -= ((cp + cl) - sp) ;
	                        sp = (cp + cl) ;
	                    } /* end while */
	                } /* end for */
	                op->hash = hash ;
	                op->fl.hash = true ;
	           } /* end if (needed) */
	        } /* end if (open) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (calent_mkhash) */

int calent_sethash(calent *op,uint hash) noex {
	int		rs ;
	if ((rs = calent_magic(op)) >= 0) {
	    rs = SR_OK ;
	    op->hash = hash ;
	    op->fl.hash = true ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (calent_sethash) */

int calent_gethash(calent *op,uint *rp) noex {
	int		rs ;
	int		f = false ;
	if ((rs = calent_magic(op)) >= 0) {
	    rs = SR_OK ;
	    f = op->fl.hash ;
	    if (rp) {
	        *rp = (f) ? op->hash : 0 ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (calent_gethash) */

int calent_loadbuf(calent *op,char *rbuf,int rlen,cchar *mp) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = calent_magic(op,rbuf,mp)) >= 0) {
	    if (sbuf b ; (rs = b.start(rbuf,rlen)) >= 0) {
	        calent_ln	*lines = op->lines ;
	        int		nlines = op->i ; /* number of line elements */
	        int		ll ;
	        cchar		*lp ;
	        for (int i = 0 ; i < nlines ; i += 1) {
	            if (i > 0) b.chr(' ') ;
	            lp = (mp + lines[i].loff) ;
	            ll = lines[i].llen ;
	            rs = b.strw(lp,ll) ;
	            if (rs < 0) break ;
	        } /* end for */
	        len = b.finish ;
	        if (rs >= 0) rs = len ;
	    } /* end if (sbuf) */
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (calent_loadbuf) */

int calent_getci(calent *op) noex {
	int		rs ;
	if ((rs = calent_magic(op)) >= 0) {
	    rs = op->cidx ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (calent_getci) */



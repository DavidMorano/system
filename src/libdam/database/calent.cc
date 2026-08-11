/* calent SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<hash.h>		/* LIBUC |hash_elf(3dam)| */
#include	<localmisc.h>		/* LIBU */

#include	"calent.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int calent_ctor(calent *op,Args ... args) noex {
    	CALENT		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (calent_ctor) */

local int calent_dtor(calent *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (calent_dtor) */

template<typename ... Args>
local inline int calent_magic(calent *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == CALENT_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (calent_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int calent_start(calent *op,calent_q *qp,uint loff,int llen) noex {
	cint		ne = CALENT_NLE ;
	int		rs ;
	if ((rs = calent_ctor(op,qp)) >= 0) ylikely {
	    cint	sz = ne * szof(calent_ln) ;
	    op->cidx = -1 ;
	    op->q = *qp ;
	    op->voff = loff ;
	    op->vlen = llen ;
	    if (calent_ln *elp ; (rs = lm_mall(sz,&elp)) >= 0) ylikely {
	        op->lines = elp ;
	        op->e = ne ;
	        op->i += 1 ;
	        op->magval = CALENT_MAGIC ;
	        elp->loff = loff ;
	        elp->llen = llen ;
	    } /* end if (memory-acquire) */
	    if (rs < 0) {
		calent_dtor(op) ;
	    } /* end if (error) */
	} /* end if (calent_ctor) */
	return rs ;
} /* end subroutine (calent_start) */

int calent_finish(calent *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = calent_magic(op)) >= 0) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->e > 0) ylikely {
		rs = SR_BADFMT ;
	        if ((op->i >= 0) && (op->i <= op->e)) ylikely {
		    rs = SR_OK ;
	            if (op->lines) {
	                rs1 = lm_free(op->lines) ;
	                if (rs >= 0) rs = rs1 ;
	                op->lines = nullptr ;
	            } /* end if (memory-release) */
	            op->i = 0 ;
	            op->e = 0 ;
	        } /* end if (good) */
	    } /* end if (open) */
	    {
		rs1 = calent_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (calent_finish) */

int calent_setidx(calent *op,int cidx) noex {
	int		rs ;
	if ((rs = calent_magic(op)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (cidx >= 0) {
	        rs = cidx ;
	        op->cidx = cidx ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (calent_setidx) */

int calent_add(calent *op,uint loff,int llen) noex {
	int		rs ;
	if ((rs = calent_magic(op)) >= 0) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->e > 0) ylikely {
		rs = SR_BADFMT ;
	        if ((op->i >= 0) && (op->i <= op->e)) ylikely {
	            calent_ln	*elp ;
	            int		sz ;
		    rs = SR_OK ;
	            if (op->i == op->e) {
	                cint	ne = (op->e * 2) + CALENT_NLE ;
	                sz = ne * szof(calent_ln) ;
	                if ((rs = lm_rall(op->lines,sz,&elp)) >= 0) {
	                    op->e = ne ;
	                    op->lines = elp ;
	                }
	            }
	            if (rs >= 0) ylikely {
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
} /* end subroutine (calent_add) */

int calent_samecite(calent *op,CALENT *oop) noex {
	int		rs ;
	int		f = true ;
	if ((rs = calent_magic(op,oop)) >= 0) ylikely {
	    f = f && (op->q.y == oop->q.y) ;
	    f = f && (op->q.m == oop->q.m) ;
	    f = f && (op->q.d == oop->q.d) ;
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (calent_samecite) */

int calent_mkhash(calent *op,cchar *md) noex {
	int		rs ;
	if ((rs = calent_magic(op,md)) >= 0) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->e > 0) ylikely {
		rs = SR_NOTOPEN ;
	        if (op->lines) ylikely {
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
	                        sl -= intconv((cp + cl) - sp) ;
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
} /* end subroutine (calent_mkhash) */

int calent_sethash(calent *op,uint hash) noex {
	int		rs ;
	if ((rs = calent_magic(op)) >= 0) ylikely {
	    rs = SR_OK ;
	    op->hash = hash ;
	    op->fl.hash = true ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (calent_sethash) */

int calent_gethash(calent *op,uint *rp) noex {
	int		rs ;
	int		f = false ;
	if ((rs = calent_magic(op)) >= 0) ylikely {
	    rs = SR_OK ;
	    f = op->fl.hash ;
	    if (rp) {
	        *rp = (f) ? op->hash : 0 ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (calent_gethash) */

int calent_loadbuf(calent *op,char *rbuf,int rlen,cchar *mp) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = calent_magic(op,rbuf,mp)) >= 0) ylikely {
	    if (sbuf b ; (rs = b.start(rbuf,rlen)) >= 0) ylikely {
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
} /* end subroutine (calent_loadbuf) */

int calent_getci(calent *op) noex {
	int		rs ;
	if ((rs = calent_magic(op)) >= 0) ylikely {
	    rs = op->cidx ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (calent_getci) */



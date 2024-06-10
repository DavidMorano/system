/* calent SUPPORT */
/* lang=C20 */

/* calendar entry object (for CALYEARS) */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We manage an individual calendar entry.  We do not actually
	hold the entry (proper).  Rather we hold a reference to the
	entry.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<estrings.h>
#include	<sbuf.h>
#include	<sfx.h>
#include	<hash.h>		/* |hash_elf(3dam)| */
#include	<localmisc.h>

#include	"calent.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* exported variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int calent_start(calent *ep,calent_q *qp,uint loff,int llen) noex {
	int		rs = SR_FAULT ;
	if (ep && qp) {
	    calent_ln	*elp ;
	    cint		ne = CALENT_NLE ;
	    int		sz ;
	    memclear(ep) ;
	    ep->cidx = -1 ;
	    ep->q = *qp ;
	    ep->voff = loff ;
	    ep->vlen = llen ;
	    sz = ne * sizeof(calent_ln) ;
	    if ((rs = uc_malloc(sz,&elp)) >= 0) {
	        ep->lines = elp ;
	        ep->e = ne ;
	        ep->i += 1 ;
	        ep->magic = 0x99000001 ;
	        elp->loff = loff ;
	        elp->llen = llen ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (calent_start) */

int calent_finish(calent *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (ep == nullptr) return SR_FAULT ;

	if (ep->e <= 0) return SR_NOTOPEN ;

	if ((ep->i < 0) || (ep->i > ep->e)) return SR_BADFMT ;

	if (ep->lines != nullptr) {
	    rs1 = uc_free(ep->lines) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->lines = nullptr ;
	}

	ep->i = 0 ;
	ep->e = 0 ;
	return rs ;
}
/* end subroutine (calent_finish) */

int calent_setidx(calent *ep,int cidx) noex {
	if (ep == nullptr) return SR_FAULT ;

	ep->cidx = cidx ;
	return SR_OK ;
}
/* end subroutine (calent_setidx) */

int calent_add(calent *ep,uint loff,int llen) noex {
	calent_ln	*elp ;
	int		rs = SR_OK ;
	int		ne ;
	int		sz ;

	if (ep == nullptr) return SR_FAULT ;

	if (ep->e <= 0) return SR_NOTOPEN ;

	if ((ep->i < 0) || (ep->i > ep->e)) return SR_BADFMT ;

	if (ep->i == ep->e) {
	    ne = (ep->e * 2) + CALENT_NLE ;
	    sz = ne * sizeof(calent_ln) ;
	    if ((rs = uc_realloc(ep->lines,sz,&elp)) >= 0) {
	        ep->e = ne ;
	        ep->lines = elp ;
	    }
	}

	if (rs >= 0) {
	    ep->vlen = ((loff + llen) - ep->voff) ;
	    elp = (ep->lines + ep->i) ;
	    elp->loff = loff ;
	    elp->llen = llen ;
	    ep->i += 1 ;
	}

	return rs ;
}
/* end subroutine (calent_add) */

int calent_samecite(calent *ep,CALENT *oep) noex {
	int		f = true ;
	f = f && (ep->q.y == oep->q.y) ;
	f = f && (ep->q.m == oep->q.m) ;
	f = f && (ep->q.d == oep->q.d) ;
	return f ;
}
/* end subroutine (calent_samecite) */

int calent_mkhash(calent *ep,cchar *md) noex {
	calent_ln	*elp = ep->lines ;
	int		rs = SR_OK ;

	if (ep == nullptr) return SR_FAULT ;

	if (ep->e <= 0) return SR_NOTOPEN ;

	if (ep->lines == nullptr) return SR_NOTOPEN ;

	if (! ep->f.hash) {
	    uint	hash = 0 ;
	    int		i ;
	    int		sl, cl ;
	    cchar	*sp, *cp ;
	        for (i = 0 ; i < ep->i ; i += 1) {
	            sp = (md + elp[i].loff) ;
	            sl = elp[i].llen ;
	            while ((cl = nextfield(sp,sl,&cp)) > 0) {
	                hash += hash_elf(cp,cl) ;
	                sl -= ((cp + cl) - sp) ;
	                sp = (cp + cl) ;
	            } /* end while */
	        } /* end for */
	        ep->hash = hash ;
	        ep->f.hash = true ;
	} /* end if (needed) */

	return rs ;
}
/* end subroutine (calent_mkhash) */

int calent_sethash(calent *ep,uint hash) noex {
	ep->hash = hash ;
	ep->f.hash = true ;
	return SR_OK ;
}
/* end subroutine (calent_sethash) */

int calent_gethash(calent *ep,uint *rp) noex {
	int		rs = SR_OK ;
	int		f = ep->f.hash ;
	if (rp != nullptr) {
	    *rp = (f) ? ep->hash : 0 ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (calent_gethash) */

int calent_loadbuf(calent *ep,char *rbuf,int rlen,cchar *mp) noex {
	sbuf		b ;
	int		rs ;
	int		len = 0 ;
	if ((rs = sbuf_start(&b,rbuf,rlen)) >= 0) {
	    calent_ln	*lines = ep->lines ;
	    int		nlines = ep->i ; /* number of line elements */
	    int		ll ;
	    cchar	*lp ;
	    for (int i = 0 ; i < nlines ; i += 1) {
	        if (i > 0) sbuf_chr(&b,' ') ;
	        lp = (mp + lines[i].loff) ;
	        ll = lines[i].llen ;
	        rs = sbuf_strw(&b,lp,ll) ;
	        if (rs < 0) break ;
	    } /* end for */
	    len = sbuf_finish(&b) ;
	    if (rs >= 0) rs = len ;
	} /* end if (sbuf) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (calent_loadbuf) */

int calent_getci(calent *ep) noex {
	int		rs = SR_FAULT ;
	if (ep) {
	    rs = ep->cidx ;
	}
	return rs ;
}
/* end subroutine (calent_getci) */



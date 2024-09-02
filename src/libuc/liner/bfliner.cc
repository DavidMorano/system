/* bfliner SUPPORT */
/* lang=C++20 */

/* BFILE-liner */
/* version %I% last-modified %G% */

#define	CF_BFLINERADV	0		/* |bfliner_adv()| ? */

/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object provider a way to read lines w/ a push-back
	feature. This is used for reading raw mail-message files
	for analysis.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<char.h>
#include	<localmisc.h>

#include	"bfliner.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* exported subroutines */

int bfliner_start(bfliner *op,bfile *ifp,off_t foff,int to) noex {
	int		rs = SR_FAULT ;
	if (op && ifp) {
	    char	*cp{} ;
	    op->ifp = ifp ;
	    op->poff = 0 ;
	    op->foff = foff ;
	    op->to = to ;
	    op->ll = 0 ;
	    if ((rs = malloc_ml(&cp)) >= 0) {
		op->lbuf = cp ;
		op->llen = rs ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bfliner_start) */

int bfliner_finish(bfliner *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    op->ll = 0 ;
	    if (op->lbuf) {
	        rs1 = uc_free(op->lbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        op->lbuf = nullptr ;
		op->llen = 0 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bfliner_finish) */

int bfliner_readpending(bfliner *op) noex {
	int		rs = SR_FAULT ;
	int		f = false ;
	if (op) {
	    rs = SR_OK ;
	    f = (op->ll >= 0) ;
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (bfliner_readpending) */

int bfliner_readln(bfliner *op,int llen,cchar **lpp) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->ll < 0) {
	        bfile	*ifp = op->ifp ;
	        op->poff = op->foff ;
	        rs = breadlnto(ifp,op->lbuf,llen,op->to) ;
	        len = rs ;
	        op->ll = len ;
	        if (rs > 0) {
	            op->foff += len ;
	        }
	    } else {
	        len = op->ll ;
	    } /* end if (needed a new line) */
	    if (lpp) {
	        *lpp = (rs >= 0) ? op->lbuf : nullptr ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (bfliner_readln) */

int bfliner_readover(bfliner *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->ll = 0 ;
	    op->lbuf[0] = '\0' ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bfliner_readover) */

int bfliner_getpoff(bfliner *op,off_t *rp) noex {
	int		rs = SR_FAULT ;
	if (op && rp) {
	    rs = SR_OK ;
	    *rp = op->poff ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bfliner_getpoff) */

#if	CF_BFLINERADV
static int bfliner_adv(bfliner *op,int inc) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->poff = op->foff ;
	    if (inc > 0) {
	        if (! op->f_reg) {
	            int		rlen = inc ;
	            while ((rs >= 0) && (rlen > 0)) {
	                cint	mlen = min(rlen,op->llen) ;
	                rs = filer_read(&op->mfb,op->lbuf,mlen,op->to) ;
	                rlen -= rs ;
	            } /* end while */
	        } else {
	            rs = filer_adv(&op->mfb,inc) ;
		}
	        op->foff += inc ;
	    } /* end if */
	    op->ll = -1 ;
	    op->lbuf[0] = '\0' ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bfliner_adv) */
#endif /* CF_BFLINERADV */



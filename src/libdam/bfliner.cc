/* bfliner SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* BFILE-liner */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	bfliner

	Description:
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

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


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

int bfliner_getln(bfliner *op,cchar **lpp) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op) {
	    bfile	*ifp = op->ifp ;
	    op->poff = op->foff ;
	    if ((rs = breadlnto(ifp,op->lbuf,op->llen,op->to)) >= 0) {
	        len = rs ;
	        op->ll = len ;
	        op->foff += len ;
	    } /* end if (breadlnto) */
	    if (lpp) {
	        *lpp = (rs >= 0) ? op->lbuf : nullptr ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (bfliner_getln) */

int bfliner_getlns(bfliner *op,cchar **lpp) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op) {
	    cnullptr	np{} ;
	    bfile	*ifp = op->ifp ;
	    op->poff = op->foff ;
	    if ((rs = breadlns(ifp,op->lbuf,op->llen,op->to,np)) >= 0) {
	        len = rs ;
	        op->ll = len ;
	        op->foff += len ;
	    } /* end if (breadlnto) */
	    if (lpp) {
	        *lpp = (rs >= 0) ? op->lbuf : nullptr ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (bfliner_getlns) */

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

int bfliner_adv(bfliner *op,int inc) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->poff = op->foff ;
	    if (inc > 0) {
	        bfile	*ifp = op->ifp ;
		int	rlen = inc ;
		while ((rs >= 0) && (rlen > 0)) {
		    cint	ml = min(op->llen,rlen) ;
	            if ((rs = bread(ifp,op->lbuf,ml)) >= 0) {
	                op->foff += rs ;
			rlen -= rs ;
		    }
		} /* end while */
	    } /* end if */
	    op->ll = -1 ;
	    op->lbuf[0] = '\0' ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bfliner_adv) */

int bfliner::start(bfile *fp,off_t so,int to) noex {
	return bfliner_start(this,fp,so,to) ;
}

int bfliner::getln(cchar **lpp) noex {
	return bfliner_getln(this,lpp) ;
}

int bfliner::getlns(cchar **lpp) noex {
	return bfliner_getlns(this,lpp) ;
}

int bfliner::getpoff(off_t *offp) noex {
	return bfliner_getpoff(this,offp) ;
}

int bfliner::adv(int inc) noex {
	return bfliner_adv(this,inc) ;
}

void bfliner::dtor() noex {
	cint	rs = int(finish) ;
	if (rs < 0) {
	    ulogerror("bfliner",rs,"fini-finish") ;
	}
} /* end method (bfliner::dtor) */

bfliner_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case bflinermem_readpending:
	        rs = bfliner_readpending(op) ;
	        break ;
	    case bflinermem_readover:
	        rs = bfliner_readover(op) ;
	        break ;
	    case bflinermem_finish:
	        rs = bfliner_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (bfliner_co::operator) */



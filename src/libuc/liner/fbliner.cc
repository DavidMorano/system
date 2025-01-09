/* fbliner SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* file-buf (now FILER) file-read line handling */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-01-10, David A­D­ Morano
        This is being written as a helper line-reading object
	for use in other objects.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	fbliner

	Description:
	This object is a line-reading helper manager.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<filer.h>
#include	<localmisc.h>

#include	"fbliner.h"


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


/* local variables */


/* exported variables */


/* exported subroutines */

int fbliner_start(fbliner *lsp,filer *fbp,off_t foff,int to) noex {
	int		rs = SR_FAULT ;
	if (lsp && fbp) {
	    if (char *lp{} ; (rs = malloc_ml(&lp)) >= 0) {
		lp[0] = '\0' ;
	        lsp->llen = rs ;
	        lsp->lbuf = lp ;
	        lsp->poff = 0 ;
	        lsp->foff = foff ;
	        lsp->fbp = fbp ;
	        lsp->to = to ;
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fbliner_start) */

int fbliner_finish(fbliner *lsp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (lsp) {
	    rs = SR_OK ;
	    lsp->llen = 0 ;
	    lsp->lbuf[0] = '\0' ;
	    {
	        rs1 = uc_free(lsp->lbuf) ;
		if (rs >= 0) rs = rs1 ;
	        lsp->lbuf = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fbliner_finish) */

int fbliner_getln(fbliner *lsp,cchar **lpp) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (lsp) {
	    filer	*fbp = lsp->fbp ;
	    rs = SR_OK ;
	    if (lsp->llen < 0) {
	        cint	to = lsp->to ;
	        cint	ll = lsp->llen ;
	        char	*lp = lsp->lbuf ;
	        lsp->poff = lsp->foff ;
	        if ((rs = filer_readln(fbp,lp,ll,to)) >= 0) {
		    len = rs ;
	            lsp->foff += len ;
	        }
	    } /* end if (needed a new line) */
	    if (lpp) {
	        *lpp = (rs >= 0) ? lsp->lbuf : nullptr ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (fbliner_getln) */

int fbliner_done(fbliner *lsp) noex {
	int		rs = SR_FAULT ;
	if (lsp) {
	    rs = SR_OK ;
	    lsp->poff = lsp->foff ;
	    lsp->llen = -1 ;
	    lsp->lbuf[0] = '\0' ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fbliner_done) */

int fbliner_adv(fbliner *lsp,int inc) noex {
    	int		rs = SR_FAULT ;
	if (lsp) {
	    rs = SR_OK ;
	    lsp->poff = lsp->foff ;
	    if (inc > 0) {
	        lsp->llen = -1 ;
	        lsp->lbuf[0] = '\0' ;
	        lsp->poff += inc ;
	        lsp->foff += inc ;
	        rs = filer_adv(lsp->fbp,inc) ;
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fbliner_adv) */

int fbliner::start(filer *fbp,off_t off,int to) noex {
	return fbliner_start(this,fbp,off,to) ;
}

int fbliner::getln(cchar **lpp) noex {
	return fbliner_getln(this,lpp) ;
}

void fbliner::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("fbliner",rs,"fini-finish") ;
	}
}

int fbliner_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case fblinermem_done:
	        rs = fbliner_done(op) ;
	        break ;
	    case fblinermem_adv:
	        rs = fbliner_adv(op,a) ;
	        break ;
	    case fblinermem_finish:
	        rs = fbliner_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (fbliner_co::operator) */



/* fbliner SUPPORT */
/* lang=C++20 */

/* special file-read line handling */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-01-10, David A­D­ Morano
        This is being written from scratch to finally get an abstracted
        "mailbox" that is fast enough for interactive use.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	fbliner

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
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

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


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
	    char	*lp{} ;
	    if ((rs = malloc_ml(&lp)) >= 0) {
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

int fbliner_read(fbliner *lsp,cchar **lpp) noex {
	filer		*fbp = lsp->fbp ;
	int		rs = SR_OK ;
	int		len = 0 ;
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
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (fbliner_read) */

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

int fbliner_seek(fbliner *lsp,int inc) noex {
	int		rs = SR_OK ;
	lsp->poff = lsp->foff ;
	if (inc > 0) {
	    lsp->llen = -1 ;
	    lsp->lbuf[0] = '\0' ;
	    lsp->poff += inc ;
	    lsp->foff += inc ;
	    rs = filer_adv(lsp->fbp,inc) ;
	} /* end if */
	return rs ;
}
/* end subroutine (fbliner_seek) */



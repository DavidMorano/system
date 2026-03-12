/* cmbuf SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Connection Manager Buffer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	cmbuf

	Description:
	This is a communications connection manager object.  This
	object abstracts the details of a particular connection
	from the calling program.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<mkpathx.h>
#include	<ischarx.h>		/* |iseol(3uc)| */
#include	<localmisc.h>

#include	"cmbuf.h"


/* local defines */

#define	CB		cmbuf
#define	CB_SP		cmbuf_sp


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local inline int cmbuf_magic(cmbuf *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magval == CMBUF_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (cmbuf_magic) */

local bool isend(int ch) noex {
    ch &= UCHAR_MAX ;
    return iseol(ch) ;
} /* end subroutine (isend) */


/* local variables */


/* exported variables */


/* exported subroutines */

int cmbuf_start(CB *op,cchar *sp,int sl) noex {
    	int		rs = SR_FAULT ;
	if (op && sp) {
	    rs = SR_INVALID ;
	    if (sl > 0) {
	        op->buf = charp(sp) ;
	        op->bp = charp(sp) ;
	        op->bl = 0 ;
	        op->buflen = sl ;
	        op->magval = CMBUF_MAGIC ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cmbuf_start) */

int cmbuf_finish(CB *op) noex {
    	int		rs ;
	if ((rs = cmbuf_magic(op)) >= 0) {
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cmbuf_finish) */

int cmbuf_getspace(CB *op,CB_SP *bdrp) noex {
    	int		rs ;
	int		len = 0 ;
	if ((rs = cmbuf_magic(op,bdrp)) >= 0) {
	    if (op->bl > 0) {
	        memmove(op->buf,op->bp,op->bl) ;
	    }
	    op->bp = op->buf ;
	    bdrp->bp = op->buf + op->bl ;
	    bdrp->bl = op->buflen - op->bl ;
	    len = bdrp->bl ;
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (cmbuf_space) */

int cmbuf_added(CB *op,int len) noex {
    	int		rs ;
	if ((rs = cmbuf_magic(op)) >= 0) {
	    op->bl += len ;
	    len = op->bl ;
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (cmbuf_added) */

int cmbuf_getline(CB *op,int llen,cchar **lpp) noex {
    	int		rs ;
	int		len = 0 ;
	if ((rs = cmbuf_magic(op,lpp)) >= 0) {
	    rs = SR_INVALID ;
	    if (llen >= 0) {
	        int	bl ;
	        int	maxll ;
	        int	f_eol = false ;
	        int	f_again = true ;
	        cchar	*sbp, *ebp ;
	        char	*bp ;
	        maxll = min(llen,op->buflen) ;
	        bp = op->bp ;
	        bl = op->bl ;
	        *lpp = op->bp ;
	        sbp = op->bp ;
	        ebp = (op->bp + bl) ;
	        while ((bp < ebp) && ((bp - sbp) < maxll)) {
	            f_eol = isend(bp[0]) ;
	            bp += 1 ;
	            if (f_eol) break ;
	        } /* end while */
	        len = intconv(bp - sbp) ;
	        if (f_eol || (len == maxll)) {
	            f_again = false ;
	        }
	        if ((! f_again) && (bp < ebp)) {
	            if ((! f_eol) && isend(bp[0])) {
		        bp += 1 ;
		        len += 1 ;
	            }
	            if ((len > 0) && (bp[-1] == '\r') && 
		        (bp < ebp) && (bp[0] == '\n')) {
		            bp += 1 ;
		            len += 1 ;
	            }
	        } /* end if */
	        if (f_again) {
	            rs = SR_AGAIN ;
	        } else {
	            op->bl -= len ;
	            op->bp = bp ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (cmbuf_getline) */

int cmbuf_getlastline(CB *op,cchar **lpp) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = cmbuf_magic(op)) >= 0) {
	    len = op->bl ;
	    if (lpp) {
	        *lpp = op->bp ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (cmbuf_getlastline) */


/* local subroutines */



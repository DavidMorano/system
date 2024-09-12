/* fsi SUPPORT */
/* lang=C++20 */

/* FIFO-String-Interlocked management */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object manages interlocked FIFO-string operations.

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<usystem.h>
#include	<ptm.h>
#include	<fifostr.h>
#include	<localmisc.h>

#include	"fsi.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int fsi_ctor(fsi *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    op->qsp = nullptr ;
	    if ((op->mxp = new(nothrow) ptm) != np) {
	        if ((op->qsp = new(nothrow) fifostr) != np) {
		    rs = SR_OK ;
		} /* end if (new-fifostr) */
	        if (rs < 0) {
		    delete op->mxp ;
		    op->mxp = nullptr ;
		}
	    } /* end if (new-ptm) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fsi_ctor) */

static int fsi_dtor(fsi *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->qsp) {
		delete op->qsp ;
		op->qsp = nullptr ;
	    }
	    if (op->mxp) {
		delete op->mxp ;
		op->mxp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fsi_dtor) */


/* local variables */


/* exported variables */


/* exported subroutines */

int fsi_start(fsi *op) noex {
	int		rs ;
	if ((rs = fsi_ctor(op)) >= 0) {
	    if ((rs = ptm_create(op->mxp,nullptr)) >= 0) {
	        rs = fifostr_start(op->qsp) ;
	        if (rs < 0) {
	            ptm_destroy(op->mxp) ;
	        }
	    }
	    if (rs < 0) {
		fsi_dtor(op) ;
	    }
	} /* end if (ptm_ctor) */
	return rs ;
}
/* end subroutine (fsi_start) */

int fsi_finish(fsi *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->qsp) {
	        rs1 = fifostr_finish(op->qsp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->mxp) {
	        rs1 = ptm_destroy(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = fsi_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fsi_finish) */

int fsi_add(fsi *op,cchar *sbuf,int slen) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if ((rs = ptm_lock(op->mxp)) >= 0) {
	    {
	        rs = fifostr_add(op->qsp,sbuf,slen) ;
		rv = rs ;
	    }
	    rs1 = ptm_unlock(op->mxp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (fsi_add) */

int fsi_remove(fsi *op,char *sbuf,int slen) noex {
	int		rs ;
	int		rs1 ;
	int		rl = 0 ;
	if ((rs = ptm_lock(op->mxp)) >= 0) {
	    {
	        rs = fifostr_remove(op->qsp,sbuf,slen) ;
	        rl = rs ;
	    }
	    rs1 = ptm_unlock(op->mxp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (fsi_remove) */

int fsi_rem(fsi *op,char *sbuf,int slen) noex {
	return fsi_remove(op,sbuf,slen) ;
}
/* end subroutine (fsi_rem) */

int fsi_count(fsi *op) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = ptm_lock(op->mxp)) >= 0) {
	    {
	        rs = fifostr_count(op->qsp) ;
	        c = rs ;
	    }
	    rs1 = ptm_unlock(op->mxp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (fsi_count) */



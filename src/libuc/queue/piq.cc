/* piq SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* pointer interlocked queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Object:
	piq

	Description:
	This is a pointer Q. The caller must supply entries with
	the first two (pointer) compnents of the entry available
	for pointer use by this object.

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<ptm.h>
#include	<pq.h>
#include	<localmisc.h>

#include	"piq.h"


/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int piq_ctor(piq *op,Args ... args) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    if ((op->mxp = new(nothrow) ptm) != np) {
	        if ((op->qlp = new(nothrow) pq) != np) {
		    rs = SR_OK ;
	        } /* end if (new-pq) */
		if (rs < 0) {
		    delete op->mxp ;
		    op->mxp = nullptr ;
	        }
	    } /* end if (new-ptm) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (piq_ctor) */

static inline int piq_dtor(piq *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->qlp) {
		delete op->qlp ;
		op->qlp = nullptr ;
	    }
	    if (op->mxp) {
		delete op->mxp ;
		op->mxp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (piq_dtor) */

template<typename ... Args>
static inline int piq_magic(piq *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == PIQ_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (piq_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int piq_start(piq *op) noex {
	int		rs = SR_FAULT ;
	if ((rs = piq_ctor(op)) >= 0) {
	    if ((rs = ptm_create(op->mxp,nullptr)) >= 0) {
	        if ((rs = pq_start(op->qlp)) >= 0) {
		    op->magic = PIQ_MAGIC ;
	        }
	        if (rs < 0)
		    ptm_destroy(op->mxp) ;
	    }
	    if (rs < 0) {
		piq_dtor(op) ;
	    }
	} /* end if (piq_ctor) */
	return rs ;
}
/* end subroutine (piq_start) */

int piq_finish(piq *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = piq_magic(op)) >= 0) {
	    {
	        rs1 = pq_finish(op->qlp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ptm_destroy(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = piq_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (piq_finish) */

int piq_ins(piq *op,void *vp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = piq_magic(op,vp)) >= 0) {
	        if ((rs = ptm_lock(op->mxp)) >= 0) {
	            {
		        pq_ent	*pep = (pq_ent *) vp ;
	                rs = pq_ins(op->qlp,pep) ;
	                c = rs ;
	            }
	            rs1 = ptm_unlock(op->mxp) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (mutex) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (piq_ins) */

int piq_rem(piq *op,void *vrp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = piq_magic(op)) >= 0) {
	    void	**vpp = (void **) vrp ;
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
	        if (pq_ent *pep{} ; (rs = pq_remtail(op->qlp,&pep)) >= 0) {
	            c = rs ;
	            if (vpp) *vpp = pep ;
	        }
	        rs1 = ptm_unlock(op->mxp) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (piq_rem) */

int piq_count(piq *op) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = piq_magic(op)) >= 0) {
	        if ((rs = ptm_lock(op->mxp)) >= 0) {
	            {
	                rs = pq_count(op->qlp) ;
	                c = rs ;
	            }
	            rs1 = ptm_unlock(op->mxp) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (mutex) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (piq_count) */

int piq_audit(piq *op) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = piq_magic(op)) >= 0) {
	        if ((rs = ptm_lock(op->mxp)) >= 0) {
	            {
	                rs = pq_audit(op->qlp) ;
		        c = rs ;
    	            }
	            rs1 = ptm_unlock(op->mxp) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (mutex) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (piq_audit) */



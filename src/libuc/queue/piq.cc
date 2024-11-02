/* piq SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* pointer interlocked queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	This is a pointer Q. The caller must supply entries with
	the first two (pointer) compnents of the entry available
	for pointer use by this object.

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<ptm.h>
#include	<pq.h>

#include	"piq.h"


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
static inline int piq_ctor(piq *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    const nullptr_t	np{} ;
	    rs = SR_NOMEM ;
	    if ((op->mxp = new(nothrow) ptm) != np) {
	        if ((op->freep = new(nothrow) pq) != np) {
		    rs = SR_OK ;
	        } /* end if (new-pq) */
		if (rs < 0) {
		    delete op->mxp ;
		    op->mxp = nullptr ;
	        }
	    } /* end if (new-ptm) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (piq_ctor) */

static inline int piq_dtor(piq *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->freep) {
		delete op->freep ;
		op->freep = nullptr ;
	    }
	    if (op->mxp) {
		delete op->mxp ;
		op->mxp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (piq_dtor) */

template<typename ... Args>
static inline int piq_magic(piq *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == PIQ_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}

static int	pq_finishup(pq *) noex ;


/* exported variables */


/* exported subroutines */

int piq_start(piq *op) noex {
	int		rs = SR_FAULT ;
	if ((rs = piq_ctor(op)) >= 0) {
	    if ((rs = ptm_create(op->mxp,nullptr)) >= 0) {
	        if ((rs = pq_start(op->freep)) >= 0) {
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
	        rs1 = pq_finishup(op->freep) ;
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
	                rs = pq_ins(op->freep,pep) ;
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
	    pq_ent	*pep{} ;
	    void	**vpp = (void **) vrp ;
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
	            if ((rs = pq_remtail(op->freep,&pep)) >= 0) {
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
	                rs = pq_count(op->freep) ;
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
	                rs = pq_audit(op->freep) ;
		        c = rs ;
    	            }
	            rs1 = ptm_unlock(op->mxp) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (mutex) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (piq_audit) */


/* private subroutines */

static int pq_finishup(pq *qp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    pq_ent	*pep{} ;
	    while ((rs1 = pq_rem(qp,&pep)) >= 0) {
	        rs1 = uc_free(pep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if ((rs >= 0) && (rs1 != SR_EMPTY)) rs = rs1 ;
	}
	{
	    rs1 = pq_finish(qp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (pq_finishup) */



/* piq */
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
#include	<sys/types.h>
#include	<usystem.h>
#include	<ptm.h>
#include	<pq.h>
#include	<localmisc.h>

#include	"piq.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	pq_finishup(pq *) noex ;

template<typename ... Args>
static inline int piq_magic(piq *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == PIQ_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}


/* exported subroutines */

int piq_start(piq *qhp) noex {
	int		rs = SR_FAULT ;
	if (qhp) {
	    if ((rs = ptm_create(&qhp->m,nullptr)) >= 0) {
	        if ((rs = pq_start(&qhp->frees)) >= 0) {
		    qhp->magic = PIQ_MAGIC ;
	        }
	        if (rs < 0)
		    ptm_destroy(&qhp->m) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (piq_start) */

int piq_finish(piq *qhp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = piq_magic(qhp)) >= 0) {
	        {
	            rs1 = pq_finishup(&qhp->frees) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        {
	            rs1 = ptm_destroy(&qhp->m) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        qhp->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (piq_finish) */

int piq_ins(piq *qhp,void *vp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = piq_magic(qhp,vp)) >= 0) {
	        if ((rs = ptm_lock(&qhp->m)) >= 0) {
	            {
		        pq_ent	*pep = (pq_ent *) vp ;
	                rs = pq_ins(&qhp->frees,pep) ;
	                c = rs ;
	            }
	            rs1 = ptm_unlock(&qhp->m) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (mutex) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (piq_ins) */

int piq_rem(piq *qhp,void *vrp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = piq_magic(qhp)) >= 0) {
	    pq_ent	*pep{} ;
	    void	**vpp = (void **) vrp ;
	    if ((rs = ptm_lock(&qhp->m)) >= 0) {
	            if ((rs = pq_remtail(&qhp->frees,&pep)) >= 0) {
	                c = rs ;
	                if (vpp) *vpp = pep ;
	            }
	            rs1 = ptm_unlock(&qhp->m) ;
		    if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (piq_rem) */

int piq_count(piq *qhp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = piq_magic(qhp)) >= 0) {
	        if ((rs = ptm_lock(&qhp->m)) >= 0) {
	            {
	                rs = pq_count(&qhp->frees) ;
	                c = rs ;
	            }
	            rs1 = ptm_unlock(&qhp->m) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (mutex) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (piq_count) */

int piq_audit(piq *qhp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = piq_magic(qhp)) >= 0) {
	        if ((rs = ptm_lock(&qhp->m)) >= 0) {
	            {
	                rs = pq_audit(&qhp->frees) ;
		        c = rs ;
    	            }
	            rs1 = ptm_unlock(&qhp->m) ;
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



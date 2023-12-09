/* cq */
/* lang=C20 */

/* container Q */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-17, David A­D­ Morano
	Oh what a cheap Q!  I do not know why I am doing this!

	= 2017-11-21, David A­D­ Morano
	Added new method |cq_unlink()|.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module implement a simple cheap queue (CQ).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<usystem.h>
#include	<vechand.h>
#include	<localmisc.h>

#include	"cq.h"


/* local defines */


/* exported subroutines */

int cq_start(cq *cqp) noex {
	const int	vo = (VECHAND_OORDERED | VECHAND_OCOMPACT) ;
	const int	de = CQ_DEFENTS ;
	int		rs ;

	if (cqp == NULL) return SR_FAULT ;

	if ((rs = vechand_start(&cqp->q,de,vo)) >= 0) {
	    cqp->magic = CQ_MAGIC ;
	}

	return rs ;
}
/* end subroutine (cq_start) */


int cq_finish(cq *cqp)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (cqp == NULL) return SR_FAULT ;
	if (cqp->magic != CQ_MAGIC) return SR_NOTOPEN ;

	rs1 = vechand_finish(&cqp->q) ;
	if (rs >= 0) rs = rs1 ;

	cqp->magic = 0 ;
	return rs ;
}
/* end subroutine (cq_finish) */


/* insert at tail */
int cq_ins(cq *cqp,void *ep)
{
	int		rs ;

	if (cqp == NULL) return SR_FAULT ;
	if (cqp->magic != CQ_MAGIC) return SR_NOTOPEN ;

	rs = vechand_add(&cqp->q,ep) ;

	return rs ;
}
/* end subroutine (cq_ins) */


/* remove from head */
int cq_rem(cq *cqp,void *vp)
{
	int		rs ;
	int		count = 0 ;

	if (cqp == NULL) return SR_FAULT ;
	if (vp == NULL) return SR_FAULT ;
	if (cqp->magic != CQ_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_get(&cqp->q,0,vp)) >= 0) {
	    vechand_del(&cqp->q,0) ;
	    count = vechand_count(&cqp->q) ;
	}

	return (rs >= 0) ? count : rs ;
}
/* end subroutine (cq_rem) */


/* unlink entry from where ever it is (if it is) */
int cq_unlink(cq *cqp,void *vp)
{
	int		rs ;
	int		count = 0 ;

	if (cqp == NULL) return SR_FAULT ;
	if (vp == NULL) return SR_FAULT ;
	if (cqp->magic != CQ_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_ent(&cqp->q,vp)) >= 0) {
	    vechand_del(&cqp->q,0) ;
	    count = vechand_count(&cqp->q) ;
	}

	return (rs >= 0) ? count : rs ;
}
/* end subroutine (cq_unlink) */

int cq_count(cq *cqp) noex {
	int		rs = SR_FAULT ;
	if (cqp && curp) {
	    rs = SR_NOTOPEN ;
	    if (cqp->magic == CQ_MAGIC) {
		rs = vechand_count(&cqp->q) ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cq_count) */

int cq_curbegin(cq *cqp,CQ_CUR *curp) noex {
	int		rs = SR_FAULT ;
	if (cqp && curp) {
	    rs = SR_NOTOPEN ;
	    if (cqp->magic == CQ_MAGIC) {
		rs = SR_OK ;
		curp->i = -1 ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cq_curbegin) */

int cq_curend(cq *cqp,CQ_CUR *curp) noex {
	int		rs = SR_FAULT ;
	if (cqp && curp) {
	    rs = SR_NOTOPEN ;
	    if (cqp->magic == CQ_MAGIC) {
		rs = SR_OK ;
		curp->i = -1 ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cq_curend) */

int cq_enum(cq *cqp,CQ_CUR *curp,void *vrp) noex {
	int		rs = SR_FAULT ;
	if (cqp && vrp) {
	    rs = SR_NOTOPEN ;
	    if (cqp->magic == CQ_MAGIC) {
	        int	i = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	        void	*vp{} ;
	        if ((rs = vechand_get(&cqp->q,i,&vp)) >= 0) {
		    void	**rpp = (void **) vrp ;
		    *rpp = vp ;
	    	    curp->i = i ;
		}
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cq_enum) */



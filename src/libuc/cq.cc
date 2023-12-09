/* cq */
/* lang=C++20 */

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


/* forward referenes */

template<typename ... Args>
static inline int cq_magic(cq *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == CQ_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}



/* exported subroutines */

int cq_start(cq *cqp) noex {
	int		rs = SR_FAULT ;
	if (cqp) {
	    cint	vo = (VECHAND_OORDERED | VECHAND_OCOMPACT) ;
	    cint	de = CQ_DEFENTS ;
	    if ((rs = vechand_start(&cqp->q,de,vo)) >= 0) {
	        cqp->magic = CQ_MAGIC ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cq_start) */

int cq_finish(cq *cqp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = cq_magic(cqp)) >= 0) {
		{
		    rs1 = vechand_finish(&cqp->q) ;
		    if (rs >= 0) rs = rs1 ;
		}
		cqp->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cq_finish) */

int cq_ins(cq *cqp,void *ep) noex {
	int		rs ;
	if ((rs = cq_magic(cqp,ep)) >= 0) {
		rs = vechand_add(&cqp->q,ep) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cq_ins) */

int cq_rem(cq *cqp,void *vrp) noex {
	int		rs ;
	int		count = 0 ;
	if ((rs = cq_magic(cqp)) >= 0) {
		void	*vp{} ;
		if ((rs = vechand_get(&cqp->q,0,&vp)) >= 0) {
		    void	**rpp = (void **) vrp ;
		    if (rpp) *rpp = vp ;
	    	    vechand_del(&cqp->q,0) ;
	            count = vechand_count(&cqp->q) ;
	        }
	} /* end if (magic) */
	return (rs >= 0) ? count : rs ;
}
/* end subroutine (cq_rem) */

int cq_unlink(cq *cqp,void *ep) noex {
	int		rs ;
	int		count = 0 ;
	if ((rs = cq_magic(cqp,ep)) >= 0) {
		if ((rs = vechand_ent(&cqp->q,ep)) >= 0) {
	    	    vechand_del(&cqp->q,0) ;
	    	    count = vechand_count(&cqp->q) ;
		}
	} /* end if (magic) */
	return (rs >= 0) ? count : rs ;
}
/* end subroutine (cq_unlink) */

int cq_count(cq *cqp) noex {
	int		rs ;
	if ((rs = cq_magic(cqp)) >= 0) {
		rs = vechand_count(&cqp->q) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cq_count) */

int cq_curbegin(cq *cqp,cq_cur *curp) noex {
	int		rs ;
	if ((rs = cq_magic(cqp,curp)) >= 0) {
		curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cq_curbegin) */

int cq_curend(cq *cqp,cq_cur *curp) noex {
	int		rs ;
	if ((rs = cq_magic(cqp,curp)) >= 0) {
		curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cq_curend) */

int cq_enum(cq *cqp,cq_cur *curp,void *vrp) noex {
	int		rs ;
	if ((rs = cq_magic(cqp,curp,vrp)) >= 0) {
	        int	i = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	        void	*vp{} ;
	        if ((rs = vechand_get(&cqp->q,i,&vp)) >= 0) {
		    void	**rpp = (void **) vrp ;
		    *rpp = vp ;
	    	    curp->i = i ;
		}
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cq_enum) */



/* cq SUPPORT */
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
#include	<usystem.h>
#include	<vechand.h>

#include	"cq.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* local structures */


/* forward referenes */

template<typename ... Args>
static inline int cq_ctor(cq *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    const nullptr_t	np{} ;
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    if ((op->qp = new(nothrow) vechand) != np) {
		rs = SR_OK ;
	    } /* end if (new-pq) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cq_ctor) */

static inline int cq_dtor(cq *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->qp) {
		delete op->qp ;
		op->qp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cq_dtor) */

template<typename ... Args>
static int cq_magic(cq *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == CQ_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}


/* local variables */


/* exported variables */


/* exported subroutines */

int cq_start(cq *op) noex {
	int		rs ;
	if ((rs = cq_ctor(op)) >= 0) {
	    cint	vo = (VECHAND_OORDERED | VECHAND_OCOMPACT) ;
	    cint	de = CQ_DEFENTS ;
	    if ((rs = vechand_start(op->qp,de,vo)) >= 0) {
	        op->magic = CQ_MAGIC ;
	    }
	    if (rs < 0) {
		cq_dtor(op) ;
	    }
	} /* end if (cq_ctor) */
	return rs ;
}
/* end subroutine (cq_start) */

int cq_finish(cq *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = cq_magic(op)) >= 0) {
	    {
		rs1 = vechand_finish(op->qp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = cq_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cq_finish) */

int cq_ins(cq *op,void *ep) noex {
	int		rs ;
	if ((rs = cq_magic(op,ep)) >= 0) {
		rs = vechand_add(op->qp,ep) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cq_ins) */

int cq_rem(cq *op,void *vrp) noex {
	int		rs ;
	int		count = 0 ;
	if ((rs = cq_magic(op)) >= 0) {
		void	*vp{} ;
		if ((rs = vechand_get(op->qp,0,&vp)) >= 0) {
		    void	**rpp = (void **) vrp ;
		    if (rpp) *rpp = vp ;
	    	    vechand_del(op->qp,0) ;
	            count = vechand_count(op->qp) ;
	        }
	} /* end if (magic) */
	return (rs >= 0) ? count : rs ;
}
/* end subroutine (cq_rem) */

int cq_unlink(cq *op,void *ep) noex {
	int		rs ;
	int		count = 0 ;
	if ((rs = cq_magic(op,ep)) >= 0) {
		if ((rs = vechand_ent(op->qp,ep)) >= 0) {
	    	    vechand_del(op->qp,0) ;
	    	    count = vechand_count(op->qp) ;
		}
	} /* end if (magic) */
	return (rs >= 0) ? count : rs ;
}
/* end subroutine (cq_unlink) */

int cq_count(cq *op) noex {
	int		rs ;
	if ((rs = cq_magic(op)) >= 0) {
		rs = vechand_count(op->qp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cq_count) */

int cq_curbegin(cq *op,cq_cur *curp) noex {
	int		rs ;
	if ((rs = cq_magic(op,curp)) >= 0) {
		curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cq_curbegin) */

int cq_curend(cq *op,cq_cur *curp) noex {
	int		rs ;
	if ((rs = cq_magic(op,curp)) >= 0) {
		curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cq_curend) */

int cq_enum(cq *op,cq_cur *curp,void *vrp) noex {
	int		rs ;
	if ((rs = cq_magic(op,curp,vrp)) >= 0) {
	        int	i = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	        void	*vp{} ;
	        if ((rs = vechand_get(op->qp,i,&vp)) >= 0) {
		    void	**rpp = (void **) vrp ;
		    *rpp = vp ;
	    	    curp->i = i ;
		}
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cq_enum) */



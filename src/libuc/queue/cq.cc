/* cq SUPPORT */
/* charset=ISO8859-1 */
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

  	Object
	cq

	Description:
	This module implement a simple cheap queue (CQ).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<ulogerror.h>
#include	<uclibmem.h>
#include	<vechand.h>
#include	<localmisc.h>

#include	"cq.h"


/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */


/* local structures */


/* forward referenes */

template<typename ... Args>
local inline int cq_ctor(cq *op,Args ... args) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    op->magval = 0 ;
	    if ((op->qp = new(nothrow) vechand) != np) ylikely {
		rs = SR_OK ;
	    } /* end if (new-pq) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (cq_ctor) */

local inline int cq_dtor(cq *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->qp) ylikely {
		delete op->qp ;
		op->qp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (cq_dtor) */

template<typename ... Args>
local int cq_magic(cq *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == CQ_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (cq_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int cq_start(cq *op) noex {
	int		rs ;
	if ((rs = cq_ctor(op)) >= 0) ylikely {
	    cint	vo = (vechandm.ordered | vechandm.compact) ;
	    cint	de = CQ_DEFENTS ;
	    if ((rs = vechand_start(op->qp,de,vo)) >= 0) ylikely {
	        op->magval = CQ_MAGIC ;
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
	if ((rs = cq_magic(op)) >= 0) ylikely {
	    if (op->qp) ylikely {
		rs1 = vechand_finish(op->qp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = cq_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cq_finish) */

int cq_ins(cq *op,void *ep) noex {
	int		rs ;
	if ((rs = cq_magic(op,ep)) >= 0) ylikely {
	    rs = vechand_add(op->qp,ep) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cq_ins) */

int cq_rem(cq *op,void *vrp) noex {
	int		rs ;
	int		count = 0 ;
	if ((rs = cq_magic(op)) >= 0) ylikely {
	    if (void *vp ; (rs = vechand_get(op->qp,0,&vp)) >= 0) ylikely {
		void	**rpp = voidpp(vrp) ;
		if (rpp) *rpp = vp ;
	    	vechand_del(op->qp,0) ;
	        count = vechand_count(op->qp) ;
	    } /* end if (vechand_get) */
	} /* end if (magic) */
	return (rs >= 0) ? count : rs ;
}
/* end subroutine (cq_rem) */

int cq_unlink(cq *op,void *ep) noex {
	int		rs ;
	int		count = 0 ;
	if ((rs = cq_magic(op,ep)) >= 0) ylikely {
	    if ((rs = vechand_ent(op->qp,ep)) >= 0) ylikely {
		vechand_del(op->qp,0) ;
		count = vechand_count(op->qp) ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? count : rs ;
}
/* end subroutine (cq_unlink) */

int cq_count(cq *op) noex {
	int		rs ;
	if ((rs = cq_magic(op)) >= 0) ylikely {
	    rs = vechand_count(op->qp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cq_count) */

int cq_curbegin(cq *op,cq_cur *curp) noex {
	int		rs ;
	if ((rs = cq_magic(op,curp)) >= 0) ylikely {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cq_curbegin) */

int cq_curend(cq *op,cq_cur *curp) noex {
	int		rs ;
	if ((rs = cq_magic(op,curp)) >= 0) ylikely {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cq_curend) */

int cq_curenum(cq *op,cq_cur *curp,void *vrp) noex {
	int		rs ;
	if ((rs = cq_magic(op,curp,vrp)) >= 0) ylikely {
	    int	i = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	    if (void *vp ; (rs = vechand_get(op->qp,i,&vp)) >= 0) ylikely {
		void	**rpp = voidpp(vrp) ;
		*rpp = vp ;
	    	curp->i = i ;
	    } /* end if (vechand_get) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cq_curenum) */

int cq::ins(void *ep) noex {
	return cq_ins(this,ep) ;
}

int cq::rem(void *rpp) noex {
	return cq_rem(this,rpp) ;
}

int cq::unlink(void *rpp) noex {
	return cq_unlink(this,rpp) ;
}

int cq::curbegin(cq_cur *curp) noex {
	return cq_curbegin(this,curp) ;
}

int cq::curend(cq_cur *curp) noex {
	return cq_curend(this,curp) ;
}

int cq::curenum(cq_cur *curp,void *rpp) noex {
	return cq_curenum(this,curp,rpp) ;
}

void cq::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("cq",rs,"fini-finish") ;
	}
} /* end method (cq::dtor) */

cq::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (this) ylikely {
	    rs = cq_count(this) ;
	} /* end if (non-null) */
	return rs ;
} /* end method (cq::operator) */

cq_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case cqmem_start:
	        rs = cq_start(op) ;
	        break ;
	    case cqmem_count:
	        rs = cq_count(op) ;
	        break ;
	    case cqmem_finish:
	        rs = cq_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (cq_co::operator) */





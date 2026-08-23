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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ulogerror.h>		/* LIBU */
#include	<ptm.h>			/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<pq.h>			/* LIBUC */
#include	<localmisc.h>		/* LIBU */

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
local inline int piq_ctor(piq *op,Args ... args) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    if ((op->mxp = new(nothrow) ptm) != np) ylikely {
	        if ((op->qlp = new(nothrow) pq) != np) ylikely {
		    rs = SR_OK ;
	        } /* end if (new-pq) */
		if (rs < 0) {
		    delete op->mxp ;
		    op->mxp = nullptr ;
	        } /* end if (error) */
	    } /* end if (new-ptm) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (piq_ctor) */

local inline int piq_dtor(piq *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->qlp) ylikely {
		delete op->qlp ;
		op->qlp = nullptr ;
	    }
	    if (op->mxp) ylikely {
		delete op->mxp ;
		op->mxp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (piq_dtor) */

template<typename ... Args>
local inline int piq_magic(piq *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == PIQ_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (piq_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int piq_start(piq *op) noex {
	int		rs ;
	if ((rs = piq_ctor(op)) >= 0) ylikely {
	    ptm *mxp = op->mxp ;
	    if ((rs = mxp->create) >= 0) ylikely {
	        if ((rs = pq_start(op->qlp)) >= 0) ylikely {
		    op->magval = PIQ_MAGIC ;
	        } /* end if */
	        if (rs < 0) {
		    mxp->destroy() ;
		} /* end if (error) */
	    } /* end if */
	    if (rs < 0) {
		piq_dtor(op) ;
	    } /* end if (error) */
	} /* end if (piq_ctor) */
	return rs ;
} /* end subroutine (piq_start) */

int piq_finish(piq *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = piq_magic(op)) >= 0) ylikely {
	    if (op->qlp) ylikely {
	        rs1 = pq_finish(op->qlp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->mxp) ylikely {
	        ptm *mxp = op->mxp ;
	        rs1 = mxp->destroy ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = piq_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (piq_finish) */

int piq_ins(piq *op,void *vp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = piq_magic(op,vp)) >= 0) ylikely {
	    ptm *mxp = op->mxp ;
	    if ((rs = mxp->lockbegin) >= 0) ylikely {
	        {
		    pq_ent	*pep = (pq_ent *) vp ;
	            rs = pq_ins(op->qlp,pep) ;
	            c = rs ;
	        }
	        rs1 = mxp->lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (piq_ins) */

int piq_rem(piq *op,void *vrp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = piq_magic(op)) >= 0) ylikely {
	    void	**vpp = (void **) vrp ;
	    ptm *mxp = op->mxp ;
	    if ((rs = mxp->lockbegin) >= 0) ylikely {
	        if (pq_ent *pep{} ; (rs = pq_remtail(op->qlp,&pep)) >= 0) {
	            c = rs ;
	            if (vpp) *vpp = pep ;
	        }
	        rs1 = mxp->lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (piq_rem) */

int piq_count(piq *op) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = piq_magic(op)) >= 0) ylikely {
	    ptm *mxp = op->mxp ;
	    if ((rs = mxp->lockbegin) >= 0) ylikely {
	        {
	            rs = pq_count(op->qlp) ;
	            c = rs ;
	        }
	        rs1 = mxp->lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (piq_count) */

int piq_audit(piq *op) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = piq_magic(op)) >= 0) ylikely {
	    ptm *mxp = op->mxp ;
	    if ((rs = mxp->lockbegin) >= 0) ylikely {
	        {
	            rs = pq_audit(op->qlp) ;
		    c = rs ;
	        }
	        rs1 = mxp->lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (piq_audit) */

int piq::ins(void *ep) noex {
	return piq_ins(this,ep) ;
} /* end method */

int piq::rem(void *rpp) noex {
	return piq_rem(this,rpp) ;
} /* end method */

void piq::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("piq",rs,"fini-finish") ;
	}
} /* end method (piq::dtor) */

piq::operator int () noex {
	return piq_count(this) ;
} /* end method (piq::operator) */

piq_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case piqmem_start:
	        rs = piq_start(op) ;
	        break ;
	    case piqmem_count:
	        rs = piq_count(op) ;
	        break ;
	    case piqmem_audit:
	        rs = piq_audit(op) ;
	        break ;
	    case piqmem_finish:
	        rs = piq_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (piq_co::operator) */



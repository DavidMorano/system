/* ciq SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* circular (really doubly linked) interlocked queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-27, David A­D­ Morano
	This module was adapted from the PPI/LPPI OS code.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	ciq

	Description:
	This is a container object that implements FIFO access.  It
	is interlocked for multi-thread use.

	Notes:
	Note that we delete all of the entries upon the object
	itself being freed.  If the entries are not opaque (as they
	usually are not), this will result in lost memory (memory
	leaks).  It is the responsibility of the caller the delete
	any non-opaque element objects.

*******************************************************************************/

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

#include	"ciq.h"


/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct ciq_ent : pq_ent {
	void		*vp ;		/* caller supplied pointer */
} ;


/* forward references */

template<typename ... Args>
static inline int ciq_ctor(ciq *op,Args ... args) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    if ((op->mxp = new(nothrow) ptm) != np) {
	        if ((op->fifop = new(nothrow) pq) != np) {
	            if ((op->freep = new(nothrow) pq) != np) {
		        rs = SR_OK ;
	            } /* end if (new-pq) */
		    if (rs < 0) {
		        delete op->fifop ;
		        op->fifop = nullptr ;
	            }
	        } /* end if (new-pq) */
		if (rs < 0) {
		    delete op->mxp ;
		    op->mxp = nullptr ;
	        }
	    } /* end if (new-ptm) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ciq_ctor) */

static inline int ciq_dtor(ciq *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->freep) {
		delete op->freep ;
		op->freep = nullptr ;
	    }
	    if (op->fifop) {
		delete op->fifop ;
		op->fifop = nullptr ;
	    }
	    if (op->mxp) {
		delete op->mxp ;
		op->mxp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ciq_dtor) */

template<typename ... Args>
static int ciq_magic(ciq *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == CIQ_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}

static int	ciq_findent(ciq *,pq_ent **,cvoid *) noex ;

static int	pq_finishup(pq *) noex ;


/* exported variables */


/* exported variables */


/* exported subroutines */

int ciq_start(ciq *op) noex {
	int		rs ;
	if ((rs = ciq_ctor(op)) >= 0) {
	    if ((rs = ptm_create(op->mxp,nullptr)) >= 0) {
		if ((rs = pq_start(op->fifop)) >= 0) {
	            if ((rs = pq_start(op->freep)) >= 0) {
		        op->magic = CIQ_MAGIC ;
		    }
		    if (rs < 0) {
		        pq_finish(op->fifop) ;
		    }
	        } /* end if (pq_start) */
	        if (rs < 0) {
		    ptm_destroy(op->mxp) ;
		}
	    } /* end if (ptm_create) */
	    if (rs < 0) {
		ciq_dtor(op) ;
	    }
	} /* end if (ciq_ctor) */
	return rs ;
}
/* end subroutine (ciq_start) */

int ciq_finish(ciq *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = ciq_magic(op)) >= 0) {
	    {
	        rs1 = pq_finishup(op->freep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = pq_finishup(op->fifop) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ptm_destroy(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ciq_finish) */

int ciq_ins(ciq *op,void *vp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = ciq_magic(op,vp)) >= 0) {
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
	        pq_ent	*pep{} ; /* used-afterwards */
	        cint	rse = SR_EMPTY ;
	        if ((rs = pq_remtail(op->freep,&pep)) == rse) {
		    cint	esz = szof(ciq_ent) ;
	            rs = libmem.mall(esz,&pep) ;
	        } /* end if (memory-allocation) */
	        if (rs >= 0) {
	            ciq_ent	*cep = (ciq_ent *) pep ;
	            cep->vp = vp ;
	            rs = pq_ins(op->fifop,pep) ;
	            c = rs ;
	        } /* end if */
	        rs1 = ptm_unlock(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (ciq_ins) */

int ciq_rem(ciq *op,void *vrp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = ciq_magic(op,vrp)) >= 0) {
	    void	**vpp = voidpp(vrp) ;
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
	        if (pq_ent *pep{} ; (rs = pq_rem(op->fifop,&pep)) >= 0) {
	            c = rs ;
	            if (vpp) {
		        ciq_ent	*cep = (ciq_ent *) pep ;
	                *vpp = cep->vp ;
	            } /* end if */
		    rs1 = pq_ins(op->freep,pep) ;
		    if (rs1 < 0) {
		        libmem.free(pep) ;
		    }
	        } /* end if (pq_rem) */
	        rs1 = ptm_unlock(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (ciq_rem) */

int ciq_gettail(ciq *op,void *vrp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = ciq_magic(op)) >= 0) {
	    void	**rpp = (void **) vrp ;
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
	        if (pq_ent *pep{} ; (rs = pq_gettail(op->fifop,&pep)) >= 0) {
	            c = rs ;
	            if (rpp) {
		        ciq_ent	*cep = (ciq_ent *) pep ;
	                *rpp = cep->vp ;
	            }
	        } /* end if (pq_gettail) */
	        rs1 = ptm_unlock(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (ciq_gettail) */

int ciq_remtail(ciq *op,void *vrp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = ciq_magic(op)) >= 0) {
	    void	**rpp = (void **) vrp ;
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
	        if (pq_ent *pep{} ; (rs = pq_remtail(op->fifop,&pep)) >= 0) {
	            c = rs ;
	            if (rpp) {
		        ciq_ent	*cep = (ciq_ent *) pep ;
	                *rpp = cep->vp ;
		    }
		    rs = pq_ins(op->freep,pep) ;
		    if (rs < 0) {
		        libmem.free(pep) ;
		    }
	        } /* end if (pq_remtail) */
	        rs1 = ptm_unlock(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (ciq_remtail) */

int ciq_rement(ciq *op,void *ep) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = ciq_magic(op)) >= 0) {
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
	        if (pq_ent *pep{} ; (rs = ciq_findent(op,&pep,ep)) > 0) {
	            if ((rs = pq_unlink(op->fifop,pep)) >= 0) {
	                c = rs ;
		        rs = pq_ins(op->freep,pep) ;
		        if (rs < 0) {
			    libmem.free(pep) ;
			}
	            } /* end if (pq_remtail) */
	        } else if (rs == 0) {
		    rs = SR_NOTFOUND ;
	        }
	        rs1 = ptm_unlock(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (ciq_rement) */

int ciq_count(ciq *op) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = ciq_magic(op)) >= 0) {
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
	        {
	            rs = pq_count(op->fifop) ;
	            c = rs ;
	        }
	        rs1 = ptm_unlock(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (ciq_count) */

int ciq_audit(ciq *op) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if ((rs = ciq_magic(op)) >= 0) {
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
	        if ((rs = pq_audit(op->freep)) >= 0) {
	            rs = pq_audit(op->fifop) ;
		    rv = rs ;
	        }
	        rs1 = ptm_unlock(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (ciq_audit) */


/* private subroutines */

static int ciq_findent(ciq *op,pq_ent **rpp,cvoid *vrp) noex {
	pq		*pqp = op->fifop ;
	int		rs ;
	int		rs1 ;
	int		f = false ;
	if (pq_cur cur ; (rs = pq_curbegin(pqp,&cur)) >= 0) {
	    pq_ent *pep{} ; /* used-afterwards */
	    while ((rs1 = pq_curenum(pqp,&cur,&pep)) >= 0) {
		ciq_ent	*cep = (ciq_ent *) pep ;
		f = (cep->vp == vrp) ;
		if (f) break ;
	    } /* end while */
	    if ((rs >= 0) && (rs1 != SR_NOTFOUND)) rs = rs1 ;
	    if (rpp) {
		*rpp = (f) ? pep : nullptr ;
	    }
	    rs1 = pq_curend(pqp,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (pq-cur) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ciq_findent) */

static int pq_finishup(pq *qp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    if (pq_ent *pep ; (rs1 = pq_rem(qp,&pep)) >= 0) {
	        rs1 = libmem.free(pep) ;
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



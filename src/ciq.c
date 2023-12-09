/* ciq */
/* lang=C++20 */

/* container interlocked queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-27, David A­D­ Morano
	This module was adapted from the PPI/LPPI OS code.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

        This is a container object that implements FIFO access. It is
        interlocked for multi-thread use.

	Usage note:

	Note that we delete all of the entries upon the object
	itself being freed. If the entries are not opaque (as they
	usually are not), this will result in lost memory (memory
	leaks). It is the responsibility of the caller the delete
	any non-opaque element objects.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<ptm.h>
#include	<pq.h>
#include	<localmisc.h>

#include	"ciq.h"


/* local defines */

#define	CIQ_ENT		struct ciq_ent


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct ciq_ent {
	pq_ent		dummy ;		/* meta */
	void		*vp ;		/* caller supplied pointer */
} ;


/* forward references */

static int	ciq_findent(ciq *,pq_ent **,cvoid *) noex ;

template<typename ... Args>
static inline int ciq_magic(ciq *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == CIQ_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}

static int	pq_finishup(pq *) noex ;


/* exported subroutines */

int ciq_start(ciq *qhp) noex {
	int		rs = SR_FAULT ;
	if (qhp) {
	    if ((rs = ptm_create(&qhp->m,NULL)) >= 0) {
	        if ((rs = pq_start(&qhp->frees)) >= 0) {
		    if ((rs = pq_start(&qhp->fifo)) >= 0) {
		        qhp->magic = CIQ_MAGIC ;
		    }
		    if (rs < 0)
		        pq_finish(&qhp->frees) ;
	        } /* end if (pq_start) */
	        if (rs < 0)
		    ptm_destroy(&qhp->m) ;
	    } /* end if (ptm_create) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ciq_start) */

int ciq_finish(ciq *qhp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = ciq_magic(qhp)) >= 0) {
	    {
	        rs1 = pq_finishup(&qhp->fifo) ;
	        if (rs >= 0) rs = rs1 ;
	    }
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
/* end subroutine (ciq_finish) */

int ciq_ins(ciq *qhp,void *vp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = ciq_magic(qhp,vp)) >= 0) {
	    if ((rs = ptm_lock(&qhp->m)) >= 0) {
	        pq_ent	*pep{} ;
	        cint	rse = SR_EMPTY ;
	        if ((rs = pq_remtail(&qhp->frees,&pep)) == rse) {
		    cint	esize = sizeof(ciq_ent) ;
	            rs = uc_libmalloc(esize,&pep) ;
	        }
	        if (rs >= 0) {
	            ciq_ent	*cep = (ciq_ent *) pep ;
	            cep->vp = vp ;
	            rs = pq_ins(&qhp->fifo,pep) ;
	            c = rs ;
	        } /* end if */
	        rs1 = ptm_unlock(&qhp->m) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (ciq_ins) */

int ciq_rem(ciq *qhp,void *vrp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = ciq_magic(qhp,vrp)) >= 0) {
	    void	**vpp = (void **) vrp ;
	    if ((rs = ptm_lock(&qhp->m)) >= 0) {
	        pq_ent	*pep{} ;
	        if ((rs = pq_rem(&qhp->fifo,&pep)) >= 0) {
	            c = rs ;
	            if (vpp) {
		        ciq_ent	*cep = (ciq_ent *) pep ;
	                *vpp = cep->vp ;
	            } /* end if */
		    rs1 = pq_ins(&qhp->frees,pep) ;
		    if (rs1 < 0)
		        uc_libfree(pep) ;
	        } /* end if (pq_rem) */
	        rs1 = ptm_unlock(&qhp->m) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (ciq_rem) */

int ciq_gettail(ciq *qhp,void *vrp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = ciq_magic(qhp)) >= 0) {
	    void	**rpp = (void **) vrp ;
	    if ((rs = ptm_lock(&qhp->m)) >= 0) {
	        pq_ent	*pep{} ;
	        if ((rs = pq_gettail(&qhp->fifo,&pep)) >= 0) {
	            c = rs ;
	            if (rpp) {
		        ciq_ent	*cep = (ciq_ent *) pep ;
	                *rpp = cep->vp ;
	            }
	        } /* end if (pq_gettail) */
	        rs1 = ptm_unlock(&qhp->m) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (ciq_gettail) */

int ciq_remtail(ciq *qhp,void *vrp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = ciq_magic(qhp)) >= 0) {
	    void	**rpp = (void **) vrp ;
	    if ((rs = ptm_lock(&qhp->m)) >= 0) {
	        pq_ent	*pep{} ;
	        if ((rs = pq_remtail(&qhp->fifo,&pep)) >= 0) {
	            c = rs ;
	            if (rpp) {
		        ciq_ent	*cep = (ciq_ent *) pep ;
	                *rpp = cep->vp ;
		    }
		    rs = pq_ins(&qhp->frees,pep) ;
		    if (rs < 0)
		        uc_libfree(pep) ;
	        } /* end if (pq_remtail) */
	        rs1 = ptm_unlock(&qhp->m) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (ciq_remtail) */

int ciq_rement(ciq *qhp,void *ep) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = ciq_magic(qhp)) >= 0) {
	    if ((rs = ptm_lock(&qhp->m)) >= 0) {
	        pq_ent	*pep{} ;
	        if ((rs = ciq_findent(qhp,&pep,ep)) > 0) {
	            if ((rs = pq_unlink(&qhp->fifo,pep)) >= 0) {
	                c = rs ;
		        rs = pq_ins(&qhp->frees,pep) ;
		        if (rs < 0)
			    uc_libfree(pep) ;
	            } /* end if (pq_remtail) */
	        } else if (rs == 0) {
		    rs = SR_NOTFOUND ;
	        }
	        rs1 = ptm_unlock(&qhp->m) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (ciq_rement) */

int ciq_count(ciq *qhp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = ciq_magic(qhp)) >= 0) {
	    if ((rs = ptm_lock(&qhp->m)) >= 0) {
	        {
	            rs = pq_count(&qhp->fifo) ;
	            c = rs ;
	        }
	        rs1 = ptm_unlock(&qhp->m) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (ciq_count) */

int ciq_audit(ciq *qhp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = ciq_magic(qhp)) >= 0) {
	    if ((rs = ptm_lock(&qhp->m)) >= 0) {
	        if ((rs = pq_audit(&qhp->frees)) >= 0) {
	            rs = pq_audit(&qhp->fifo) ;
	        }
	        rs1 = ptm_unlock(&qhp->m) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ciq_audit) */


/* private subroutines */

static int ciq_findent(ciq *qhp,pq_ent **rpp,cvoid *vrp) noex {
	pq		*pqp = &qhp->fifo ;
	pq_cur		cur ;
	int		rs ;
	int		rs1 ;
	int		f = false ;
	if ((rs = pq_curbegin(pqp,&cur)) >= 0) {
	    pq_ent	*pep{} ;
	    while ((rs1 = pq_enum(pqp,&cur,&pep)) >= 0) {
		ciq_ent	*cep = (ciq_ent *) pep ;
		f = (cep->vp == vrp) ;
		if (f) break ;
	    } /* end while */
	    if ((rs >= 0) && (rs1 != SR_NOTFOUND)) rs = rs1 ;
	    if (rpp) {
		*rpp = (f) ? pep : NULL ;
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
	    pq_ent	*pep{} ;
	    while ((rs1 = pq_rem(qp,&pep)) >= 0) {
	        rs1 = uc_libfree(pep) ;
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



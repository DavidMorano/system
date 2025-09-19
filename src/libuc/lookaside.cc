/* lookaside SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* lookaside memory management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	Module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	lookaside

	Description:
	This object provides a fix-sized pool of memory blocks for
	fast allocation and deallocation.  However, memory is never
	released back to |malloc(3c)| (the origin from which all
	memory comes) once it is allocated.  Freed blocks are,
	however, available for new allocation requests.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdint>
#include	<new>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<pq.h>
#include	<intceil.h>
#include	<localmisc.h>

#include	"lookaside.h"


/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */

struct lookaside_ch ;

typedef lookaside_ch	*chunkp ;


/* external subroutines */


/* external variables */


/* local structures */

struct lookaside_ch {
	pq_ent		dummy ;
} ;


/* forward references */

template<typename ... Args>
static int lookaside_ctor(lookaside *op,Args ... args) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    op->eap = nullptr ;
	    op->nchunks = 0 ;
	    op->esize = 0 ;
	    op->eaoff = 0 ;
	    op->n = 0 ;
	    op->i = 0 ;
	    op->nfree = 0 ;
	    op->nused = 0 ;
	    if ((op->cqp = new(nothrow) pq) != np) {
	        if ((op->esp = new(nothrow) pq) != np) {
		    rs = SR_OK ;
	        } /* end if (new-pq) */
		if (rs < 0) {
		    delete op->cqp ;
		    op->cqp = np ;
		}
	    } /* end if (new-pq) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (lookaside_ctor) */

static int lookaside_dtor(lookaside *op) noex {
	int		rs = SR_OK ;
	if (op->esp) {
	    delete op->esp ;
	    op->esp = nullptr ;
	}
	if (op->cqp) {
	    delete op->cqp ;
	    op->cqp = nullptr ;
	}
	return rs ;
}
/* end subroutine (lookaside_dtor) */

static int	lookaside_newchunk(lookaside *) noex ;


/* local variables */

constexpr int	qalign = int(2 * szof(void *)) ;


/* exported variables */


/* exported subroutines */

int lookaside_start(lookaside *op,int esize,int n) noex {
	int		rs ;
	if ((rs = lookaside_ctor(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (esize > 0) {
		cint	csz = szof(lookaside_ch) ;
		if (esize < qalign) esize = qalign ;
		if (n < 0) n = LOOKASIDE_MINENTS ;
		op->esize = esize ;
		op->n = n ;
		op->i = -1 ;
		op->eaoff = iceil(csz,qalign) ;
		if ((rs = pq_start(op->cqp)) >= 0) {
	    	    rs = pq_start(op->esp) ;
	    	    if (rs < 0) {
			pq_finish(op->cqp) ;
		    }
		} /* end if (pq_start) */
	    } /* end if (valid) */
	    if (rs < 0) {
		lookaside_dtor(op) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (lookaside_start) */

int lookaside_finish(lookaside *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if (op) {
	    pq		*cqp = op->cqp ; /* loop invariant */
	    pq_ent	*rep = nullptr ;
	    rs = SR_OK ;
	    op->eap = nullptr ;
	    while (pq_rem(cqp,&rep) >= 0) {
	        lookaside_ch	*cp = chunkp(rep) ;
	        c += 1 ;
	        op->nchunks -= 1 ;
	        rs1 = libmem.free(cp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end while */
	    {
		rs1 = pq_finish(op->esp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = pq_finish(op->cqp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = lookaside_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (lookaside_finish) */

int lookaside_get(lookaside *op,void *p) noex {
	int		rs = SR_FAULT ;
	if (op && p) {
	    rs = SR_OK ;
	    if (op->nfree > 0) {
	        pq_ent	**epp = (pq_ent **) p ;
	        if ((rs = pq_remtail(op->esp,epp)) >= 0) {
	            op->nfree -= 1 ;
	        }
	    } else {
	        caddr_t	*pp = (caddr_t *) p ;
	        if ((op->i < 0) || (op->i >= op->n)) {
	            rs = lookaside_newchunk(op) ;
	        }
	        if (rs >= 0) {
	            *pp = caddr_t(uintptr_t(op->eap) + (op->i * op->esize)) ;
	            op->i += 1 ;
	        }
	    } /* end if */
	    if (rs >= 0) op->nused += 1 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (lookaside_get) */

int lookaside_release(lookaside *op,void *p) noex {
	int		rs = SR_FAULT ;
	if (op && p) {
	    pq_ent	*ep = (pq_ent *) p ;
	    if ((rs = pq_ins(op->esp,ep)) >= 0) {
	        op->nfree += 1 ;
	        op->nused -= 1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (lookaside_release) */

int lookaside_count(lookaside *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->nused ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (lookaside_count) */

int lookaside_audit(lookaside *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = pq_audit(op->cqp)) >= 0) {
	        rs = pq_audit(op->esp) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (lookaside_audit) */


/* private subroutines */

static int lookaside_newchunk(lookaside *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    int		sz = op->eaoff + (op->n * op->esize) ;
	    if (caddr_t a ; (rs = libmem.mall(sz,&a)) >= 0) {
		pq_ent	*nep = (pq_ent *) a ;
	        if ((rs = pq_ins(op->cqp,nep)) >= 0) {
	            op->eap = caddr_t(a + op->eaoff) ;
	            op->i = 0 ;
	            op->nchunks += 1 ;
	        } /* end if (pq_ins) */
	        if (rs < 0) {
		    libmem.free(a) ;
		}
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (lookaside_newchunk) */



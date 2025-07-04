/* fifoelem SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* FIFO string operations */
/* version %I% last-modified %G% */


/* revision history :

	= 1998-12-01, David A­D­ Morano
	Module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	fifoelem

	Description:
	This object manages a FIFO of fixed sized entries and of
	fixed sized depth.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strcmp(3c)| */
#include	<usystem.h>
#include	<localmisc.h>

#include	"fifoelem.h"

import libutil ;

/* local defines */

#define	FE		fifoelem
#define	FE_ENT		fifoelem_ent
#define	FE_CUR		fifoelem_cur


/* local typedefs */

typedef const void	cv ;


/* external subroutines */


/* local structures */


/* forward referecens */

template<typename ... Args>
static inline int fifoelem_magic(fifoelem *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == FIFOELEM_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (fifoelem_magic) */

static int	entry_start(FE_ENT *,cvoid *,int) noex ;
static int	entry_finish(FE_ENT *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int fifoelem_start(fifoelem *op) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->head = op->tail = nullptr ;
	    op->n = 0 ;
	    op->magic = FIFOELEM_MAGIC ;
	}
	return rs ;
}
/* end subroutine (fifoelem_start) */

int fifoelem_finish(fifoelem *op) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != FIFOELEM_MAGIC) return SR_NOTOPEN ;

	while ((rs = fifoelem_curdel(op,nullptr)) >= 0) ;
	if (rs == SR_NOTFOUND) rs = SR_OK ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (fifoelem_finish) */

int fifoelem_add(fifoelem *op,void *usp,int usl) noex {
	cint		esize = szof(FE_ENT) ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;
	if (usp == nullptr) return SR_FAULT ;

	if (op->magic != FIFOELEM_MAGIC) return SR_NOTOPEN ;

	if (void *vp ; (rs = uc_malloc(esize,&vp)) >= 0) {
	    FE_ENT	*ep = (FIFOELEM_ENT *) vp ;
	    if ((rs = entry_start(ep,usp,usl)) >= 0) {
	        if (op->head == nullptr) {
	            op->head = ep ;
	            op->tail = ep ;
	            ep->prev = ep->next = nullptr ;
	        } else {
	            ep->prev = op->tail ;
	            (op->tail)->next = ep ;
	            op->tail = ep ;
	        }
	        op->n += 1 ;
	        if (rs < 0) {
		    uc_free(vp) ;
		}
	    } /* end if (entry_start) */
	} /* end if (m-a) */

	return (rs >= 0) ? op->n : rs ;
}
/* end subroutine (fifoelem_add) */

int fifoelem_remove(fifoelem *op,void *ebuf,int elen) noex {
	FE_ENT		*ep ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		slen ;

	if (op == nullptr) return SR_FAULT ;
	if (ebuf == nullptr) return SR_FAULT ;

	if (op->magic != FIFOELEM_MAGIC) return SR_NOTOPEN ;

/* can we give the call an entry? */

	if (op->head == nullptr)
	    return SR_NOTFOUND ;

	ep = op->head ;
	if ((elen >= 0) && (ep->dl > elen))
	    return SR_TOOBIG ;

	memcpy(ebuf,ep->dp,ep->dl) ;

	slen = ep->dl ;
	op->head = ep->next ;
	if (op->head == nullptr) {
	    op->tail = nullptr ;
	} else {
	    (op->head)->prev = nullptr ;
	}
	{
	rs1 = entry_finish(ep) ;
	if (rs >= 0) rs = rs1 ;
	}
	{
	rs1 = uc_free(ep) ;
	if (rs >= 0) rs = rs1 ;
	}
	op->n -= 1 ;
	return (rs >= 0) ? slen : rs ;
}
/* end subroutine (fifoelem_remove) */

int fifoelem_curfetch(fifoelem *op,FE_CUR *cp,FE_ENT **epp) noex {
	FE_ENT		*ep ;

	if (op == nullptr) return SR_FAULT ;
	if (epp == nullptr) return SR_FAULT ;

	if (op->magic != FIFOELEM_MAGIC) return SR_NOTOPEN ;

/* OK, do the deed */

	if ((cp == nullptr) || (cp->current == nullptr)) {
	    ep = op->head ;
	} else {
	    ep = (cp->current)->next ;
	}

	if (cp) {
	    cp->current = ep ;
	}

	if (epp) {
	    *epp = ep ;
	}

	return ((ep != nullptr) ? ep->dl : SR_NOTFOUND) ;
}
/* end subroutine (fifoelem_curfetch) */

int fifoelem_curdel(fifoelem *op,FE_CUR *cp) noex {
	FE_ENT	*ep ;
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != FIFOELEM_MAGIC) return SR_NOTOPEN ;

	/* OK, do the deed */
	if ((cp == nullptr) || (cp->current == nullptr)) {
	    ep = op->head ;
	} else {
	    ep = cp->current ;
	}

	if (ep == nullptr)
	    return SR_NOTFOUND ;

	if (cp != nullptr) {

	    if (ep->prev == nullptr) {
	        op->head = ep->next ;
	    } else {
	        (ep->prev)->next = ep->next ;
	    }

	    if (ep->next == nullptr) {
	        op->tail = ep->prev ;
	    } else {
	        (ep->next)->prev = ep->prev ;
	    }

	    cp->current = ep->prev ;

	} else {

	    op->head = ep->next ;
	    if (op->head == nullptr) {
	        op->tail = nullptr ;
	    } else {
	        (op->head)->prev = nullptr ;
	    }

	} /* end if */
	{
	rs1 = entry_finish(ep) ;
	if (rs >= 0) rs = rs1 ;
	}
	{
	rs1 = uc_free(ep) ;
	if (rs >= 0) rs = rs1 ;
	}

	op->n -= 1 ;
	return (rs >= 0) ? op->n : rs ;
}
/* end subroutine (fifoelem_curdel) */

int fifoelem_count(fifoelem *op) noex {
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != FIFOELEM_MAGIC) return SR_NOTOPEN ;

	return op->n ;
}
/* end subroutine (fifoelem_count) */

int fifoelem_curbegin(FE *op,FE_CUR *cp) noex {
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != FIFOELEM_MAGIC) return SR_NOTOPEN ;

	cp->current = nullptr ;
	return SR_OK ;
}
/* end subroutine (fifoelem_curbegin) */

int fifoelem_curend(FE *op,FE_CUR *cp) noex {
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != FIFOELEM_MAGIC) return SR_NOTOPEN ;

	cp->current = nullptr ;
	return SR_OK ;
}
/* end subroutine (fifoelem_curend) */

int fifoelem_present(fifoelem *op,cv *vsp,int vsl,fifoelem_cmp scmp) noex {
	int		rs ;
	int		rs1 ;
	int		dl = 0 ;
	if ((rs = fifoelem_magic(op,vsp)) >= 0) {
	    int		sl = vsl ;
	    cchar	*sp = charp(vsp) ;
	    if (vsl < 0) {
		sl = lenstr(sp) ;
	    }
	    if (scmp == nullptr) scmp = fifoelem_cmp(strcmp) ;
	    auto entcmp = [&sp,&sl,scmp] (FE_ENT *ep) {
		bool f = true ;
		f = f && (sl == ep->dl) ;
		f = f && scmp(sp,ep->dp) ;
		return f ;
	    } ;
	    if (FE_CUR cur ; (rs = fifoelem_curbegin(op,&cur)) >= 0) {
	        FE_ENT	*ep ;
	        while ((rs = fifoelem_curfetch(op,&cur,&ep)) >= 0) {
		    if (entcmp(ep)) {
	                dl = ep->dl ;
	                break ;
	            }
	        } /* end while */
	        rs1 = fifoelem_curend(op,&cur) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	} /* end if (magic) */
	return (rs >= 0) ? dl : rs ;
}
/* end subroutine (fifoelem_present) */


/* privatesubroutines */

static int entry_start(FE_ENT *ep,cvoid *vsp,int vsl) noex {
	int		rs ;
	if (vsl < 0) {
	    cchar	*cp = charp(vsp) ;
	    vsl = lenstr(cp) ;
	}
	ep->prev = ep->next = nullptr ;
	cchar	*sp = charp(vsp) ;
	if (cchar *cp ; (rs = uc_mallocstrw(sp,vsl,&cp)) >= 0) {
	    ep->dl = vsl ;
	    ep->dp = voidp(cp) ;
	}
	return rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(FE_ENT *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ep->dp) {
	    rs1 = uc_free(ep->dp) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->dp = nullptr ;
	}

	ep->dl = 0 ;
	return rs ;
}
/* end subroutine (entry_finish) */



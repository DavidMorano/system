/* fifoitem SUPPORT */
/* lang=C++20 */

/* FIFO string operations */
/* version %I% last-modified %G% */


/* revision history:

	- 2005-12-01, David A­D­ Morano
	Module was originally written.

*/

/* Copyright © 2005 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

        This object manages a FIFO of variable sized entries and 
	of variable depth.

*******************************************************************************/


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"fifoitem.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

typedef int	(*cmpfun_t)(cvoid *,cvoid *) noex ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

int		fifoitem_del(FIFOITEM *,fifoitem_cur *) noex ;
int		fifoitem_curbegin(FIFOITEM *,fifoitem_cur *) noex ;
int		fifoitem_curend(FIFOITEM *,fifoitem_cur *) noex ;
int		fifoitem_fetch(FIFOITEM *,fifoitem_cur *,FIFOITEM_ENT **) noex ;

template<typename ... Args>
static inline int fifoitem_magic(fifoitem *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == FIFOITEM_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (fifoitem_magic) */

static int	entry_start(FIFOITEM_ENT *,cvoid *,int) noex ;
static int	entry_finish(FIFOITEM_ENT *) noex ;
static int	defaultcmp(cchar **,cchar **) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int fifoitem_start(fifoitem *fsp) noex {
	int		rs = SR_FAULT ;
	if (fsp) {
	    fsp->head = nullptr ;
	    fsp->tail = nullptr ;
	    fsp->n = 0 ;
	    fsp->magic = FIFOITEM_MAGIC ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fifoitem_start) */

int fifoitem_finish(fifoitem *fsp) noex {
	int		rs ;
	if ((rs = fifoitem_magic(fsp)) >= 0) {
	    while ((rs = fifoitem_del(fsp,nullptr)) >= 0) ;
	    if (rs == SR_NOTFOUND) rs = SR_OK ;
	    fsp->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fifoitem_finish) */

int fifoitem_ins(fifoitem *fsp,cvoid *sp,int sl) noex {
	cint		esize = sizeof(FIFOITEM_ENT) ;
	int		rs ;
	void		*vp ;

	if (fsp == nullptr) return SR_FAULT ;
	if (sp == nullptr) return SR_FAULT ;
	if (fsp->magic != FIFOITEM_MAGIC) return SR_NOTOPEN ;

	if ((rs = uc_malloc(esize,&vp)) >= 0) {
	    FIFOITEM_ENT	*ep = (FIFOITEM_ENT *) vp ;
	    if ((rs = entry_start(ep,sp,sl)) >= 0) {
	        if (fsp->head == nullptr) {
	            fsp->head = ep ;
	            fsp->tail = ep ;
	            ep->prev = ep->next = nullptr ;
	        } else {
	            ep->prev = fsp->tail ;
	            (fsp->tail)->next = ep ;
	            fsp->tail = ep ;
	        } /* end if */
	        fsp->n += 1 ;
	    } /* end if (entry_start) */
	    if (rs < 0) {
	        uc_free(vp) ;
	    }
	} /* end if (m-a) */

	return (rs >= 0) ? fsp->n : rs ;
}
/* end subroutine (fifoitem_ins) */

int fifoitem_rem(fifoitem *fsp,void *vbuf,int vlen) noex {
	FIFOITEM_ENT	*ep ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		dl = 0 ;

	if (fsp == nullptr) return SR_FAULT ;
	if (vbuf == nullptr) return SR_FAULT ;
	if (fsp->magic != FIFOITEM_MAGIC) return SR_NOTOPEN ;

	if (fsp->head == nullptr)
	    return SR_NOENT ;

	ep = fsp->head ;
	if ((vlen >= 0) && (ep->dl > vlen))
	    return SR_OVERFLOW ;

	dl = ep->dl ;
	memcpy(vbuf,ep->dp,ep->dl) ;

	fsp->head = ep->next ;
	if (fsp->head == nullptr) {
	    fsp->tail = nullptr ;
	} else {
	    (fsp->head)->prev = nullptr ;
	}

	rs1 = entry_finish(ep) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = uc_free(ep) ;
	if (rs >= 0) rs = rs1 ;

	fsp->n -= 1 ;
	return (rs >= 0) ? dl : rs ;
}
/* end subroutine (fifoitem_rem) */

int fifoitem_fetch(fifoitem *fsp,fifoitem_cur *curp,FIFOITEM_ENT **epp) noex {
	FIFOITEM_ENT	*ep ;
	int		rs ;

	if (fsp == nullptr) return SR_FAULT ;
	if (epp == nullptr) return SR_FAULT ;
	if (fsp->magic != FIFOITEM_MAGIC) return SR_NOTOPEN ;

/* OK, do the deed */

	if ((curp == nullptr) || (curp->current == nullptr)) {
	    ep = fsp->head ;
	} else {
	    ep = (curp->current)->next ;
	}

	if (curp != nullptr)
	    curp->current = ep ;

	if (epp != nullptr)
	    *epp = ep ;

	rs = (ep != nullptr) ? ep->dl : SR_NOTFOUND ;
	return rs ;
}
/* end subroutine (fifoitem_fetch) */

int fifoitem_del(fifoitem *fsp,fifoitem_cur *curp) noex {
	FIFOITEM_ENT	*ep ;
	int		rs = SR_OK ;
	int		rs1 ;

	if (fsp == nullptr) return SR_FAULT ;
	if (fsp->magic != FIFOITEM_MAGIC) return SR_NOTOPEN ;

/* OK, do the deed */

	if ((curp == nullptr) || (curp->current == nullptr)) {
	    ep = fsp->head ;
	} else {
	    ep = curp->current ;
	}

	if (ep != nullptr) {

	    if (curp != nullptr) {

	        if (ep->prev == nullptr) {
	            fsp->head = ep->next ;
	        } else {
	            (ep->prev)->next = ep->next ;
	        }

	        if (ep->next == nullptr) {
	            fsp->tail = ep->prev ;
	        } else {
	            (ep->next)->prev = ep->prev ;
	        }

	        curp->current = ep->prev ;

	    } else {

	        fsp->head = ep->next ;
	        if (fsp->head == nullptr) {
	            fsp->tail = nullptr ;
	        } else {
	            (fsp->head)->prev = nullptr ;
	        }

	    } /* end if */

	    rs1 = entry_finish(ep) ;
	    if (rs >= 0) rs = rs1 ;

	    rs1 = uc_free(ep) ;
	    if (rs >= 0) rs = rs1 ;

	    fsp->n -= 1 ;

	} else {
	    rs = SR_NOTFOUND ;
	}

	return (rs >= 0) ? fsp->n : rs ;
}
/* end subroutine (fifoitem_del) */

int fifoitem_count(fifoitem *fsp) noex {
	if (fsp == nullptr) return SR_FAULT ;
	if (fsp->magic != FIFOITEM_MAGIC) return SR_NOTOPEN ;

	return fsp->n ;
}
/* end subroutine (fifoitem_count) */

int fifoitem_finder(fifoitem *fsp,cchar *s,cmpfun_t cmpfunc,cchar **rpp) noex {
	fifoitem_cur	cur ;
	int		rs ;
	int		dl = 0 ;

	if (fsp == nullptr) return SR_FAULT ;
	if (fsp->magic != FIFOITEM_MAGIC) return SR_NOTOPEN ;

	if (cmpfunc == nullptr)
	    cmpfunc = (cmpfun_t) defaultcmp ;

	if ((rs = fifoitem_curbegin(fsp,&cur)) >= 0) {
	    FIFOITEM_ENT	*ep ;
	    while ((rs = fifoitem_fetch(fsp,&cur,&ep)) >= 0) {
	        if ((*cmpfunc)(s,ep->dp) == 0) {
	            dl = ep->dl ;
	            if (rpp != nullptr) {
	                cchar	*rp = (cchar *) ep->dp ;
	                *rpp = (rs >= 0) ? rp : nullptr ;
	            }
	            break ;
	        }
	    } /* end while */
	    fifoitem_curend(fsp,&cur) ;
	} /* end if (cursor) */

	return (rs >= 0) ? dl : rs ;
}
/* end subroutine (fifoitem_finder) */

int fifoitem_curbegin(fifoitem *fsp,fifoitem_cur *curp) noex {
	if (fsp == nullptr) return SR_FAULT ;
	if (fsp->magic != FIFOITEM_MAGIC) return SR_NOTOPEN ;

	curp->current = nullptr ;
	return SR_OK ;
}
/* end subroutine (fifoitem_curbegin) */

int fifoitem_curend(fifoitem *fsp,fifoitem_cur *curp) noex {
	if (fsp == nullptr) return SR_FAULT ;
	if (fsp->magic != FIFOITEM_MAGIC) return SR_NOTOPEN ;

	curp->current = nullptr ;
	return SR_OK ;
}
/* end subroutine (fifoitem_curend) */


/* private subroutines */

static int entry_start(FIFOITEM_ENT *ep,cvoid *vp,int sl) noex {
	int		rs ;
	cchar		*sp = ccharp(vp) ;
	cvoid		*dp ;

	if (sl < 0) sl = strlen(sp) ;

	ep->next = nullptr ;
	ep->prev = nullptr ;
	if ((rs = uc_mallocbuf(sp,sl,&dp)) >= 0) {
	    ep->dp = (cvoid *) dp ;
	    ep->dl = sl ;
	}

	return rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(FIFOITEM_ENT *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (ep->dp != nullptr) {
	    rs1 = uc_free(ep->dp) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->dp = nullptr ;
	}

	ep->prev = ep->next = nullptr ;
	return rs ;
}
/* end subroutine (entry_finish) */

static int defaultcmp(cchar **e1pp,cchar **e2pp) noex {
	int		rc = 0 ;
	if (*e1pp || *e2pp ) {
	    if (*e1pp == nullptr) {
	        rc = 1 ;
	    } else if (*e2pp == nullptr) {
	        rc = -1 ;
	    } else {
	        rc = strcmp(*e1pp,*e2pp) ;
	    }
	}
	return rc ;
}
/* end subroutine (defaultcmp) */



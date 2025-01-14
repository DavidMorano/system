/* fifoelem SUPPORT */
/* encoding=ISO8859-1 */
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
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"fifoelem.h"


/* local defines */

#define	FE		fifoelem
#define	FE_ENT		fifoelem_ent
#define	FE_CUR		fifoelem_cur


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward referecens */

static int	entry_start(FE_ENT *,void *,int) noex ;
static int	entry_finish(FE_ENT *) noex ;

extern "C" {
    static int	defcmp(cvoid *,cvoid *) noex ;
}



/* local variables */


/* exported variables */


/* exported subroutines */

int fifoelem_start(fifoelem *op) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->head = op->tail = NULL ;
	    op->n = 0 ;
	    op->magic = FIFOELEM_MAGIC ;
	}
	return rs ;
}
/* end subroutine (fifoelem_start) */

int fifoelem_finish(fifoelem *op) noex {
	int		rs = SR_OK ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != FIFOELEM_MAGIC) return SR_NOTOPEN ;

	while ((rs = fifoelem_curdel(op,NULL)) >= 0) ;
	if (rs == SR_NOTFOUND) rs = SR_OK ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (fifoelem_finish) */

int fifoelem_add(fifoelem *op,void *usp,int usl) noex {
	cint		esize = szof(FE_ENT) ;
	int		rs ;

	if (op == NULL) return SR_FAULT ;
	if (usp == NULL) return SR_FAULT ;

	if (op->magic != FIFOELEM_MAGIC) return SR_NOTOPEN ;

	if (void *vp ; (rs = uc_malloc(esize,&vp)) >= 0) {
	    FE_ENT	*ep = (FIFOELEM_ENT *) vp ;
	    if ((rs = entry_start(ep,usp,usl)) >= 0) {
	        if (op->head == NULL) {
	            op->head = ep ;
	            op->tail = ep ;
	            ep->previous = ep->next = NULL ;
	        } else {
	            ep->previous = op->tail ;
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

	if (op == NULL) return SR_FAULT ;
	if (ebuf == NULL) return SR_FAULT ;

	if (op->magic != FIFOELEM_MAGIC) return SR_NOTOPEN ;

/* can we give the call an entry? */

	if (op->head == NULL)
	    return SR_NOTFOUND ;

	ep = op->head ;
	if ((elen >= 0) && (ep->sl > elen))
	    return SR_TOOBIG ;

	memcpy(ebuf,ep->sp,ep->sl) ;

	slen = ep->sl ;
	op->head = ep->next ;
	if (op->head == NULL) {
	    op->tail = NULL ;
	} else {
	    (op->head)->previous = NULL ;
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

	if (op == NULL) return SR_FAULT ;
	if (epp == NULL) return SR_FAULT ;

	if (op->magic != FIFOELEM_MAGIC) return SR_NOTOPEN ;

/* OK, do the deed */

	if ((cp == NULL) || (cp->current == NULL)) {
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

	return ((ep != NULL) ? ep->sl : SR_NOTFOUND) ;
}
/* end subroutine (fifoelem_curfetch) */

int fifoelem_curdel(fifoelem *op,FE_CUR *cp) noex {
	FE_ENT	*ep ;
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != FIFOELEM_MAGIC) return SR_NOTOPEN ;

	/* OK, do the deed */
	if ((cp == NULL) || (cp->current == NULL)) {
	    ep = op->head ;
	} else {
	    ep = cp->current ;
	}

	if (ep == NULL)
	    return SR_NOTFOUND ;

	if (cp != NULL) {

	    if (ep->previous == NULL) {
	        op->head = ep->next ;
	    } else {
	        (ep->previous)->next = ep->next ;
	    }

	    if (ep->next == NULL) {
	        op->tail = ep->previous ;
	    } else {
	        (ep->next)->previous = ep->previous ;
	    }

	    cp->current = ep->previous ;

	} else {

	    op->head = ep->next ;
	    if (op->head == NULL) {
	        op->tail = NULL ;
	    } else {
	        (op->head)->previous = NULL ;
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
	if (op == NULL) return SR_FAULT ;

	if (op->magic != FIFOELEM_MAGIC) return SR_NOTOPEN ;

	return op->n ;
}
/* end subroutine (fifoelem_count) */

/* search for a string in the FIFO string object */
int fifoelem_finder(fifoelem *op,void *usp,fifoelem_f cmpfunc,void **rpp) noex {
	fifoelem_cur	cur ;
	int		rs ;
	int		rs1 ;
	int		rl = 0 ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != FIFOELEM_MAGIC) return SR_NOTOPEN ;

	if (cmpfunc == NULL)
	    cmpfunc = (fifoelem_f) defcmp ;

	if ((rs = fifoelem_curbegin(op,&cur)) >= 0) {
	    FE_ENT	*ep ;
	    while ((rs = fifoelem_curfetch(op,&cur,&ep)) >= 0) {
	        if ((*cmpfunc)(usp,(*rpp)) == 0) {
		    rl = ep->sl ;
		    if (rpp) {
	    	        *rpp = (rs >= 0) ? ep->sp : NULL ;
		    }
		    break ;
		}
	    } /* end while */
	    rs1 = fifoelem_curend(op,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cursor) */

	return ((rs >= 0) ? rl : rs) ;
}
/* end subroutine (fifoelem_finder) */

int fifoelem_curbegin(FE *op,FE_CUR *cp) noex {
	if (op == NULL) return SR_FAULT ;

	if (op->magic != FIFOELEM_MAGIC) return SR_NOTOPEN ;

	cp->current = NULL ;
	return SR_OK ;
}
/* end subroutine (fifoelem_curbegin) */

int fifoelem_curend(FE *op,FE_CUR *cp) noex {
	if (op == NULL) return SR_FAULT ;

	if (op->magic != FIFOELEM_MAGIC) return SR_NOTOPEN ;

	cp->current = NULL ;
	return SR_OK ;
}
/* end subroutine (fifoelem_curend) */


/* privatesubroutines */

static int entry_start(FE_ENT *ep,void *vsp,int vsl) noex {
	int		rs ;

	if (vsl < 0) {
	    cchar	*cp = charp(vsp) ;
	    vsl = strlen(cp) ;
	}

	ep->previous = ep->next = NULL ;
	cchar	*sp = charp(vsp) ;
	if (cchar *cp ; (rs = uc_mallocstrw(sp,vsl,&cp)) >= 0) {
	    ep->sl = vsl ;
	    ep->sp = voidp(cp) ;
	}

	return rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(FE_ENT *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ep->sp) {
	    rs1 = uc_free(ep->sp) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->sp = NULL ;
	}

	ep->sl = 0 ;
	return rs ;
}
/* end subroutine (entry_finish) */

static int defcmp(cvoid *a1p,cvoid *a2p) noex {
	cvoid		**e1pp = (cvoid **) a1p ;
	cvoid		**e2pp = (cvoid **) a2p ;
	int		rc = 0 ;
	if ((*e1pp != NULL) || (*e2pp != NULL)) {
	    if (*e1pp != NULL) {
	        if (*e2pp != NULL) {
		    cchar	*i1p = (cchar *) *e1pp ;
		    cchar	*i2p = (cchar *) *e2pp ;
	            rc = strcmp(i1p,i2p) ;
	        } else
		    rc = -1 ;
	    } else
		rc = 1 ;
	}
	return rc ;
}
/* end subroutine (defcmp) */



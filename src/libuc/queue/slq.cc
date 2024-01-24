/* slq SUPPORT */
/* lang=C++20 */

/* Circular-Pointer-Queue - regular (no-frills) pointer queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was modeled after assembly.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is a regular, pointer based, no-frills circular doubly
	linked list queue. Note that this object CANNOT be moved
	(copied) since there may be pointers pointing back at the
	list head (located in the object).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>

#include	"slq.h"


/* exported subroutines */

int slq_start(slq *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->head = nullptr ;
	    op->tail = nullptr ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (slq_start) */

int slq_finish(slq *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->head = nullptr ;
	    op->tail = nullptr ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (slq_finish) */

int slq_ins(slq *op,slq_ent *ep) noex {
	int		rs = SR_FAULT ;
	if (op && ep) {
	    ep->next = nullptr ;
	    rs = SR_OK ;
	    if (op->head && op->tail) {
	        slq_ent		*pep = op->tail ;
		pep->next = ep ;
	        op->tail = ep ;
	    } else {
		if (op->head || op->tail) {
		    rs = SR_BADFMT ;
		} else {
		    op->head = ep ;
		    op->tail = ep ;
		}
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (slq_ins) */

int slq_insgroup(slq *op,void *vp,int esz,int n) noex {
	caddr_t		ca = caddr_t(vp) ;
	int		rs = SR_FAULT ;
	if (op && ca) {
	    rs = SR_INVALID ;
	    if (esz > 0) {
	        rs = SR_OK ;
	        for (int i = 0 ; (rs == SR_OK) && (i < n) ; i += 1) {
		    slq_ent	*ep = (slq_ent *) ca ;
		    rs = slq_ins(op,ep) ;
		    ca += esz ;
	        } /* end for */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (slq_insgroup) */

int slq_rem(slq *op,slq_ent **epp) noex {
	int		rs = SR_FAULT ;
	if (op && epp) {
	    rs = SR_OK ;
	    if (op->head && op->tail) {
	        slq_ent		*hep = op->head ;
	        *epp = hep ;
		op->head = hep->next ;
	    } else {
		if (op->head || op->tail) {
		    rs = SR_BADFMT ;
		} else {
		    rs = SR_NOTFOUND ;
		}
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (slq_rem) */

int slq_gettail(slq *op,slq_ent **epp) noex {
	int		rs = SR_FAULT ;
	if (op && epp) {
	    rs = SR_OK ;
	    if (op->head && op->tail) {
	        *epp = op->tail ;
	    } else {
		if (op->head || op->tail) {
		    rs = SR_BADFMT ;
		} else {
		    rs = SR_NOTFOUND ;
		}
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (slq_gettail) */

int slq_audit(slq *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->head && op->tail) {
	        rs = SR_OK ;
	    } else {
	        if (op->head || op->tail) rs = SR_BADFMT ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (slq_audit) */



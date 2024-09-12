/* dpq SUPPORT */
/* lang=C++20 */

/* Double-Pointer-Queue - regular (no-frills) double circular pointer queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was modeled after assembly.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is a regular, pointer based, no-frills circular doubly
	linked list queue.  Note that this object CANNOT be moved
	(copied) since there may be pointers pointing back at the
	list head (located in the object).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<localmisc.h>

#include	"dpq.h"


/* local defines */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int dpq_start(dpq *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->head = nullptr ;
	    op->tail = nullptr ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dpq_start) */

int dpq_finish(dpq *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->head = nullptr ;
	    op->tail = nullptr ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dpq_finish) */

int dpq_ins(dpq *op,dpq_ent *ep) noex {
	int		rs = SR_FAULT ;
	if (op && ep) {
	    ep->next = nullptr ;
	    rs = SR_OK ;
	    if (op->head && op->tail) {
	        dpq_ent		*pep = op->tail ;
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
/* end subroutine (dpq_ins) */

int dpq_insgroup(dpq *op,void *vp,int esz,int n) noex {
	caddr_t		ca = caddr_t(vp) ;
	int		rs = SR_FAULT ;
	if (op && ca) {
	    rs = SR_INVALID ;
	    if (esz > 0) {
	        rs = SR_OK ;
	        for (int i = 0 ; (rs == SR_OK) && (i < n) ; i += 1) {
		    dpq_ent	*ep = (dpq_ent *) ca ;
		    rs = dpq_ins(op,ep) ;
		    ca += esz ;
	        } /* end for */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dpq_insgroup) */

int dpq_rem(dpq *op,dpq_ent **epp) noex {
	int		rs = SR_FAULT ;
	if (op && epp) {
	    rs = SR_OK ;
	    if (op->head && op->tail) {
	        dpq_ent		*hep = op->head ;
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
/* end subroutine (dpq_rem) */

int dpq_gettail(dpq *op,dpq_ent **epp) noex {
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
/* end subroutine (dpq_gettail) */

int dpq_audit(dpq *op) noex {
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
/* end subroutine (dpq_audit) */



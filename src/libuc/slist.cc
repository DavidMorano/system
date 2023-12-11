/* slist */
/* lang=C++20 */

/* a regular (no-frills) single-linked pointer queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-03, David A�D� Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This is a regular, pointer based, no-frills single-linked
	list queue.  Note that this object CAN be moved (copied)
	since there are no pointers pointing back at the list head
	(located in the object). This object (header) is NOT
	circularly linked.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<climits>
#include	<usystem.h>

#include	"slist.h"


/* external subroutines */


/* external variables */


/* exported subroutines */

int slist_start(SLIST *qhp) noex {
	int		rs = SR_FAULT ;
	if (qhp) {
	    qhp->head = NULL ;
	    qhp->tail = NULL ;
	    qhp->count = 0 ;
	}
	return rs ;
}
/* end subroutine (slist_start) */

int slist_finish(SLIST *qhp) noex {
	int		rs = SR_FAULT ;
	if (qhp) {
	    qhp->head = NULL ;
	    qhp->tail = NULL ;
	    qhp->count = 0 ;
	}
	return rs ;
}
/* end subroutine (slist_finish) */

int slist_ins(SLIST *qhp,SLIST_ENT *ep) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (qhp && ep) {
	    rs = SR_OK ;
	    ep->next = NULL ;
	    if (qhp->head) {
	        SLIST_ENT	*pep = qhp->tail ;
		if (pep) {
	            pep->next = ep ;
	            qhp->tail = ep ;
		} else {
		    rs = SR_BADFMT ;
		}
	    } else {
		if (qhp->tail) {
		    rs = SR_BADFMT ;
		} else {
	            qhp->head = ep ;
	            qhp->tail = ep ;
		}
	    } /* end if */
	    if (rs >= 0) {
	        qhp->count += 1 ;
	        c = qhp->count ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (slist_ins) */

int slist_insgroup(SLIST *qhp,SLIST_ENT *gp,int esize,int n) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (qhp && gp) {
	    rs = SR_INVALID ;
	    if ((esize > 0) && (n >= 0)) {
		caddr_t		p = (caddr_t) gp ;
	        rs = SR_OK ;
	        if (n > 0) {
		    SLIST_ENT	*ep = (SLIST_ENT *) p ;
	            SLIST_ENT	*pep ;
	            if (qhp->head) {
	                pep = qhp->tail ;
	                pep->next = ep ;
	            } else {
	                qhp->head = ep ;
	            } /* end if */
	            pep = ep ;
	            p += esize ;
	            for (int i = 1 ; i < n ; i += 1) {
	                ep = (SLIST_ENT *) p ;
	                pep->next = ep ;
	                p += esize ;
	                pep = ep ;
	            } /* end for */
	            pep->next = NULL ;
	            qhp->tail = pep ;
	            qhp->count += n ;
	            c = qhp->count ;
	        } /* end if (no action needed) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (slist_insgroup) */

int slist_present(SLIST *qhp,SLIST_ENT *ep) noex {
	int		rs = SR_FAULT ;
	int		f = false ;
	if (qhp && ep) {
	    SLIST_ENT	*nep = qhp->head ;
	    rs = SR_OK ;
	    while (nep && (!f)) {
		f = (ep == nep) ;
		nep = nep->next ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (slist_present) */

int slist_unlink(SLIST *qhp,SLIST_ENT *ep) noex {
	int		rs = SR_FAULT ;
	int		f = 0 ;
	if (qhp && ep) {
	    SLIST_ENT	*nep = qhp->head ;
	    SLIST_ENT	*pep = nullptr ;
	    rs = SR_OK ;
	    while (nep) {
		if ((f = (ep == nep))) break ;
		pep = nep ;
		nep = nep->next ;
	    }
	    if (f) {
		if (pep) {
		    if (ep->next) {
		        pep->next = ep->next ;
		    } else {
			qhp->tail = pep ;
			pep->next = nullptr ;
		    }
		} else {
		    if (ep->next) {
			qhp->head = ep->next ;
		    } else {
			qhp->head = nullptr ;
			qhp->tail = nullptr ;
		    }
		}
		ep->next = nullptr ;
		qhp->count -= 1 ;
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (slist_unlink) */

int slist_rem(SLIST *qhp,SLIST_ENT **epp) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (qhp && epp) {
	    SLIST_ENT	*ep = nullptr ;
	    rs = SR_EMPTY ;
	    if (qhp->head && qhp->tail) {
		ep = qhp->head ;
		if (ep->next) {
		    qhp->head = ep->next ;
		} else {
		    qhp->head = nullptr ;
		    qhp->tail = nullptr ;
		}
		qhp->count -= 1 ;
		ep->next = nullptr ;
	    } else {
		if (qhp->head || qhp->tail) rs = SR_BADFMT ;
	    } /* end if (was not empty) */
	    *epp = (rs >= 0) ? ep : NULL ;
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (slist_rem) */

int slist_gettail(SLIST *qhp,SLIST_ENT **epp) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (qhp && epp) {
	    SLIST_ENT	*ep = nullptr ;
	    rs = SR_EMPTY ;
	    if (qhp->head && qhp->tail) {
	        ep = qhp->tail ;
		if (ep->next) {
		    rs = SR_BADFMT ;
		}
	    } else {
		if (qhp->head || qhp->tail) rs = SR_BADFMT ;
	    }
	    *epp = (rs >= 0) ? ep : NULL ;
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (slist_gettail) */

int slist_count(SLIST *qhp) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (qhp) {
	    c = qhp->count ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (slist_count) */

int slist_audit(SLIST *qhp) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (qhp) {
	    if (qhp->head && qhp->tail) {
	        SLIST_ENT	*ep = qhp->head ;
		for (c = 0 ; ep && (c < INT_MAX) ; c += 1) {
		    ep = ep->next ;
	        } /* end for */
		if (c == INT_MAX) rs = SR_BADFMT ;
		if ((rs >= 0) && (c != qhp->count)) rs = SR_BADFMT ;
	    } else {
		if (qhp->head || qhp->tail) rs = SR_BADFMT ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (slist_audit) */



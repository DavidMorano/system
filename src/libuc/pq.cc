/* pq SUPPORT */
/* lang=C++23 */

/* a regular (no-frills) pointer queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-03, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is a regular, pointer based, no-frills doubly linked
	list queue.  Note that this object CAN be moved (copied)
	since there are no pointers pointing back at the list head
	(located in the object). This object (header) is NOT
	circularly linked.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<usysrets.h>
#include	<clanguage.h>

#include	"pq.h"


/* external subroutines */


/* external variables */


/* exported subroutines */

int pq_start(pq *qhp) noex {
	int		rs = SR_FAULT ;
	if (qhp) {
	    rs = SR_OK ;
	    qhp->head = nullptr ;
	    qhp->tail = nullptr ;
	    qhp->count = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pq_start) */

int pq_finish(pq *qhp) noex {
	int		rs = SR_FAULT ;
	if (qhp) {
	    rs = SR_OK ;
	    if (qhp->head && qhp->tail) {
	        qhp->head = nullptr ;
	        qhp->tail = nullptr ;
	    } else {
		if (qhp->head || qhp->tail) rs = SR_BADFMT ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pq_finish) */

int pq_ins(pq *qhp,PQ_ENT *ep) noex {
	int		rs = SR_FAULT ;
	int		rc = 0 ;
	if (qhp) {
	    rs = SR_OK ;
	    if (qhp->head && qhp->tail) {
	        if (qhp->head != qhp->tail) {
	            PQ_ENT	*pep = qhp->tail ;
	            ep->next = nullptr ;
	            ep->prev = qhp->tail ;
	            pep->next = ep ;
	            qhp->tail = ep ;
	        } else {
	            ep->next = nullptr ;
	            ep->prev = nullptr ;
	            qhp->head = ep ;
	            qhp->tail = ep ;
	        } /* end if */
	        if (rs >= 0) {
	            rc = ++qhp->count ;
	        }
	    } else if (qhp->head || qhp->tail) {
	        rs = SR_BADFMT ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (pq_ins) */

/* insert a group into queue (at the tail) */
int pq_insgroup(pq *qhp,PQ_ENT *gp,int esize,int n) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (qhp && gp) {
	    caddr_t		p = (caddr_t) gp ;
	    rs = SR_INVALID ;
	    if ((n > 0) && (esize > 0)) {
	        PQ_ENT		*ep = (PQ_ENT *) p ;
	        PQ_ENT		*pep ;
		rs = SR_OK ;
	        if (qhp->head && qhp->tail) {
		    if (qhp->head != qhp->tail) {
	                pep = qhp->tail ;
	                pep->next = ep ;
	                ep->prev = pep ;
		    } else {
	                ep->prev = nullptr ;
	                qhp->head = ep ;
		    }
	        } else {
		    if (qhp->head || qhp->tail) rs = SR_BADFMT ;
	        } /* end if */
		if (rs >= 0) {
	            pep = ep ;
	            p += esize ;
	            for (int i = 1 ; i < n ; i += 1) {
	                ep = (PQ_ENT *) p ;
	                pep->next = ep ;
	                ep->prev = pep ;
	                p += esize ;
	                pep = ep ;
	            } /* end for */
	            pep->next = nullptr ;
	            qhp->tail = pep ;
	            qhp->count += n ;
	            c = qhp->count ;
		} /* end if (ok) */
	    } else if (n == 0) {
		rs = SR_OK ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (pq_insgroup) */

int pq_gettail(pq *qhp,PQ_ENT **epp) noex {
	int		rs = SR_FAULT ;
	int		rc = 0 ;
	if (qhp) {
	    PQ_ENT	*ep = nullptr ;
	    rs = SR_EMPTY ;
	    if (qhp->head && qhp->tail) {
		rs = SR_OK ;
		ep = qhp->tail ;
	        if (ep->next) {
		    rs = SR_BADFMT ;
		} else {
	            rc = qhp->count ;
		}
	    } else {
	        if (qhp->head || qhp->tail) rs = SR_BADFMT ;
	    } /* end if (not empty) */
	    if (epp) *epp = (rs >= 0) ? ep : nullptr ;
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (pq_gettail) */

int pq_rem(pq *qhp,PQ_ENT **epp) noex {
	int		rs = SR_FAULT ;
	int		rc = 0 ;
	if (qhp) {
	    PQ_ENT	*ep = nullptr ;
	    rs = SR_EMPTY ;
	    if (qhp->head && qhp->tail) {
	        ep = qhp->head ;
		if (qhp->head != qhp->tail) {
		    rs = SR_BADFMT ;
	            if (ep->next && (ep->prev == nullptr)) {
	                PQ_ENT	*nep = ep->next ;
			rs = SR_OK ;
	                nep->prev = nullptr ;
	                qhp->head = nep ;
		    }
	        } else {
		    rs = SR_OK ;
	            qhp->head = nullptr ;
	            qhp->tail = nullptr ;
	        } /* end if */
	        if (rs >= 0) {
		    ep->next = nullptr ;
		    ep->prev = nullptr ;
		    rc = --qhp->count ;
	        }
	    } else {
	        if (qhp->head || qhp->tail) rs = SR_BADFMT ;
	    } /* end if (not-empty) */
	    if (epp != nullptr) *epp = (rs >= 0) ? ep : nullptr ;
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (pq_rem) */

/* remove from the TAIL of queue (to get "stack-like" behavior) */
int pq_remtail(pq *qhp,PQ_ENT **epp) noex {
	int		rs = SR_FAULT ;
	int		rc = 0 ;
	if (qhp) {
	    PQ_ENT	*ep = nullptr ;
	    rs = SR_EMPTY ;
	    if (qhp->head && qhp->tail) {
	        rs = SR_BADFMT ;
		ep = qhp->tail ;
		if (qhp->head != qhp->tail) {
		    rs = SR_BADFMT ;
	            if ((ep->next == nullptr) && ep->prev) {
	                PQ_ENT	*pep = ep->prev ;
			rs = SR_OK ;
	                pep->next = nullptr ;
	                qhp->tail = pep ;
		    }
	        } else {
		    rs = SR_OK ;
	            qhp->head = nullptr ;
	            qhp->tail = nullptr ;
	        }
	        if (rs >= 0) {
		    ep->next = nullptr ;
		    ep->prev = nullptr ;
		    rc = --qhp->count ;
		}
	    } else {
	        if (qhp->head || qhp->tail) rs = SR_BADFMT ;
	    } /* end if (not empty) */
	    if (epp) *epp = (rs >= 0) ? ep : nullptr ;
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (pq_remtail) */

/* we apply some special care here to make sure we actually were in the Q */
int pq_unlink(pq *qhp,PQ_ENT *ep) noex {
	int		rs = SR_FAULT ;
	int		rc = 0 ;
	if (qhp && ep) {
	     rs = SR_EMPTY ;
	     if (qhp->head && qhp->tail) {
	         PQ_ENT		*nep, *pep ;
	         rs = SR_OK ;
	         if (ep->next != nullptr) {
	             if (ep->prev != nullptr) {
	                 nep = ep->next ;
	                 nep->prev = ep->prev ;
		         pep = ep->prev ;
		         pep->next = ep->next ;
	             } else {
		         if (qhp->head == ep) {
	                     nep = ep->next ;
	                     nep->prev = nullptr ;
	                     qhp->head = nep ;
		         } else {
		             rs = SR_BADFMT ;
		         }
	             }
	         } else {
	             if (ep->prev != nullptr) {
		         if (qhp->tail == ep) {
		             pep = ep->prev ;
		             pep->next = nullptr ;
	                     qhp->tail =  pep ;
		         } else {
		             rs = SR_BADFMT ;
		         }
	             } else {
		         if ((qhp->head == ep) && (qhp->tail == ep)) {
	                     qhp->head = nullptr ;
	                     qhp->tail = nullptr ;
		         } else {
		             rs = SR_BADFMT ;
		         }
	             }
	        } /* end if */
	        if (rs >= 0) {
		    rc = --qhp->count ;
	        }
	    } else {
	        if (qhp->head || qhp->tail) rs = SR_BADFMT ;
	    } /* end if (ok) */
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (pq_unlink) */

int pq_count(pq *qhp) noex {
	int		rs = SR_FAULT ;
	int		rc = 0 ;
	if (qhp) {
	    if (qhp->head && qhp->tail) {
	        rc = qhp->count ;
	    } else {
	        if (qhp->head || qhp->tail) rs = SR_BADFMT ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (pq_count) */

int pq_audit(pq *qhp) noex {
	int		rs = SR_FAULT ;
	if (qhp) {
	    rs = SR_OK ;
	    if (qhp->head && qhp->tail) {
	        PQ_ENT		*ep = qhp->head ;
		rs = SR_BADFMT ;
	        if (ep->prev) {
	            PQ_ENT	*pep = ep ;
	            rs = SR_OK ;
	            ep = ep->next ;
	            while (ep) {
	                if (ep->prev != pep) {
	                    rs = SR_BADFMT ;
	                    break ;
	                }
	                pep = ep ;
	                ep = ep->next ;
	            } /* end while */
		    if ((rs >= 0) && (qhp->tail != pep)) {
	                rs = SR_BADFMT ;
		    }
	        } /* end if (ok) */
	    } else {
	        if (qhp->head || qhp->tail) rs = SR_BADFMT ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pq_audit) */

int pq_curbegin(pq *qhp,pq_cur *curp) noex {
	int		rs = SR_FAULT ;
	if (qhp && curp) {
	    rs = SR_OK ;
	    curp->entp = nullptr ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pq_curbegin) */

int pq_curend(pq *qhp,pq_cur *curp) noex {
	int		rs = SR_FAULT ;
	if (qhp && curp) {
	    rs = SR_OK ;
	    curp->entp = nullptr ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pq_curend) */

int pq_enum(pq *qhp,pq_cur *curp,PQ_ENT **rpp) noex {
	int		rs = SR_FAULT ;
	if (qhp && curp) {
	    rs = SR_NOTFOUND ;
	    if (qhp->head && qhp->tail) {
	        PQ_ENT	*nep ;
	        if (curp->entp != nullptr) {
	            PQ_ENT	*ep = curp->entp ;
		    nep = ep->next ;
	        } else {
	            nep = qhp->head ;
	        }
	        if (nep != nullptr) rs = SR_OK ;
	        curp->entp = nep ;
		if (rpp) *rpp = (rs >= 0) ? nep : nullptr ;
	    } else {
		if (qhp->head || qhp->tail) rs = SR_BADFMT ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pq_enum) */



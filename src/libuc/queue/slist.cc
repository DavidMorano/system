/* slist SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* a regular (no-frills) single-linked pointer queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-03, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	slist

	Description:
	This is a regular, pointer based, no-frills single-linked
	list queue.  Note that this object CAN be moved (copied)
	since there are no pointers pointing back at the list head
	(located in the object).  This object (header) is NOT
	circularly linked.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ulogerror.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"slist.h"


/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int slist_start(slist *qhp) noex {
	int		rs = SR_FAULT ;
	if (qhp) ylikely {
	    rs = SR_OK ;
	    qhp->head = nullptr ;
	    qhp->tail = nullptr ;
	    qhp->cnt = 0 ;
	}
	return rs ;
} /* end subroutine (slist_start) */

int slist_finish(slist *qhp) noex {
	int		rs = SR_FAULT ;
	if (qhp) ylikely {
	    rs = SR_OK ;
	    qhp->head = nullptr ;
	    qhp->tail = nullptr ;
	    qhp->cnt = 0 ;
	}
	return rs ;
} /* end subroutine (slist_finish) */

int slist_ins(slist *qhp,slist_ent *ep) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (qhp && ep) ylikely {
	    rs = SR_OK ;
	    ep->next = nullptr ;
	    if (qhp->head) {
	        slist_ent	*pep = qhp->tail ;
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
	    if (rs >= 0) ylikely {
	        qhp->cnt += 1 ;
	        c = qhp->cnt ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (slist_ins) */

int slist_insgroup(slist *qhp,slist_ent *gp,int esz,int n) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (qhp && gp) ylikely {
	    rs = SR_INVALID ;
	    if ((esz > 0) && (n >= 0)) ylikely {
		caddr_t		p = (caddr_t) gp ;
	        rs = SR_OK ;
	        if (n > 0) {
		    slist_ent	*ep = (slist_ent *) p ;
	            slist_ent	*pep ;
	            if (qhp->head) {
	                pep = qhp->tail ;
	                pep->next = ep ;
	            } else {
	                qhp->head = ep ;
	            } /* end if */
	            pep = ep ;
	            p += esz ;
	            for (int i = 1 ; i < n ; i += 1) {
	                ep = (slist_ent *) p ;
	                pep->next = ep ;
	                p += esz ;
	                pep = ep ;
	            } /* end for */
	            pep->next = nullptr ;
	            qhp->tail = pep ;
	            qhp->cnt += n ;
	            c = qhp->cnt ;
	        } /* end if (no action needed) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (slist_insgroup) */

int slist_present(slist *qhp,slist_ent *ep) noex {
	int		rs = SR_FAULT ;
	int		f = false ;
	if (qhp && ep) ylikely {
	    slist_ent	*nep = qhp->head ;
	    rs = SR_OK ;
	    while (nep && (!f)) {
		f = (ep == nep) ;
		nep = nep->next ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (slist_present) */

int slist_unlink(slist *qhp,slist_ent *ep) noex {
	int		rs = SR_FAULT ;
	int		f = 0 ;
	if (qhp && ep) ylikely {
	    slist_ent	*nep = qhp->head ;
	    slist_ent	*pep = nullptr ;
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
		qhp->cnt -= 1 ;
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (slist_unlink) */

int slist_rem(slist *qhp,slist_ent **epp) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (qhp && epp) ylikely {
	    slist_ent	*ep = nullptr ;
	    rs = SR_EMPTY ;
	    if (qhp->head && qhp->tail) {
		ep = qhp->head ;
		if (ep->next) {
		    qhp->head = ep->next ;
		} else {
		    qhp->head = nullptr ;
		    qhp->tail = nullptr ;
		}
		ep->next = nullptr ;
		qhp->cnt -= 1 ;
	        c = qhp->cnt ;
	    } else {
		if (qhp->head || qhp->tail) rs = SR_BADFMT ;
	    } /* end if (was not empty) */
	    *epp = (rs >= 0) ? ep : nullptr ;
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (slist_rem) */

int slist_gethead(slist *qhp,slist_ent **epp) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (qhp && epp) ylikely {
	    slist_ent	*ep = nullptr ;
	    rs = SR_EMPTY ;
	    if (qhp->head && qhp->tail) {
	        ep = qhp->head ;
	        c = qhp->cnt ;
	    } else {
		if (qhp->head || qhp->tail) rs = SR_BADFMT ;
	    }
	    *epp = (rs >= 0) ? ep : nullptr ;
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (slist_gethead) */

int slist_gettail(slist *qhp,slist_ent **epp) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (qhp && epp) ylikely {
	    slist_ent	*ep = nullptr ;
	    rs = SR_EMPTY ;
	    if (qhp->head && qhp->tail) {
	        ep = qhp->tail ;
	        c = qhp->cnt ;
		if (ep->next) {
		    rs = SR_BADFMT ;
		}
	    } else {
		if (qhp->head || qhp->tail) rs = SR_BADFMT ;
	    }
	    *epp = (rs >= 0) ? ep : nullptr ;
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (slist_gettail) */

int slist_count(slist *qhp) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (qhp) ylikely {
	    c = qhp->cnt ;
	}
	return (rs >= 0) ? c : rs ;
} /* end subroutine (slist_count) */

int slist_audit(slist *qhp) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (qhp) ylikely {
	    if (qhp->head && qhp->tail) {
	        slist_ent	*ep = qhp->head ;
		for (c = 0 ; ep && (c < INT_MAX) ; c += 1) {
		    ep = ep->next ;
	        } /* end for */
		if (c == INT_MAX) rs = SR_BADFMT ;
		if ((rs >= 0) && (c != qhp->cnt)) rs = SR_BADFMT ;
	    } else {
		if (qhp->head || qhp->tail) rs = SR_BADFMT ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (slist_audit) */


/* local subroutines */

void slist::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("slist",rs,"fini-finish") ;
	}
} /* end subroutine (slist::dtor) */

slist_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case slistmem_start:
	        rs = slist_start(op) ;
	        break ;
	    case slistmem_count:
	        rs = slist_count(op) ;
	        break ;
	    case slistmem_audit:
	        rs = slist_audit(op) ;
	        break ;
	    case slistmem_finish:
	        rs = slist_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (slist_co::operator) */

int slist::ins(slist_ent *ep) noex {
	return slist_ins(this,ep) ;
} /* end method */

int slist::insgroup(slist_ent *gp,int esz,int n) noex {
	return slist_insgroup(this,gp,esz,n) ;
} /* end method */

int slist::present(slist_ent *ep) noex {
	return slist_present(this,ep) ;
} /* end method */

int slist::unlink(slist_ent *ep) noex {
	return slist_unlink(this,ep) ;
} /* end method */

int slist::rem(slist_ent **epp) noex {
	return slist_rem(this,epp) ;
} /* end method */

int slist::gethead(slist_ent **epp) noex {
	return slist_gethead(this,epp) ;
} /* end method */

int slist::gettail(slist_ent **epp) noex {
	return slist_gettail(this,epp) ;
} /* end method */



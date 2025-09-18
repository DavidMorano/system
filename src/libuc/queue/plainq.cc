/* plainq SUPPORT */
/* charset=ISO8859-1 */
/* lang=C20 */

/* plain self-relative double-linked queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Object:
	plainq

	Description:
	This object implements a self-relative doublely linked list
	for queue operations. This type of queue can be used in
	shared memory areas that are mapped to different addresses
	in different address spaces! That is the whole idea of this
	type of a queue. This means that all queue entries are
	stored with relative positions from the head of the queue.
	This is done so that multiple processes (in different address
	spaces) can access the queue (once proper mutual exclusion
	is provided).  This object CAN be used in applications where
	the object is moved (copied) in memory since no pointers
	(whatever) point back to the header.

	Important note:

	This type of queue (used for system OS or other executive
	type purposes) requires that all entries start with the
	structure 'plainq_ent'. This is necessary because that
	structure is used for the linking of the entries into the
	Q. This is not a container object in the normal sense since
	access inside entries is required for operation! If you
	want a container Q that does not have to access the entry
	object then pick some other Q object to use instead of this
	one!

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<localmisc.h>

#include	"plainq.h"


/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */

typedef	caddr_t		ca ;
typedef plainq_ent	*entp ;


/* forward references */

template<typename ... Args>
static int plainq_magic(plainq *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == PLAINQ_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}


/* local variables */


/* exported variables */


/* exported subroutines */

int plainq_start(plainq *qhp) noex {
	int		rs = SR_FAULT ;
	if (qhp) {
	    rs = SR_OK ;
	    qhp->head = 0 ;
	    qhp->tail = 0 ;
	    qhp->count = 0 ;
	    qhp->magic = PLAINQ_MAGIC ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (plainq_start) */

int plainq_finish(plainq *qhp) noex {
	int		rs ;
	if ((rs = plainq_magic(qhp)) >= 0) {
	        qhp->head = 0 ;
	        qhp->tail = 0 ;
	        qhp->count = 0 ;
	        qhp->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (plainq_finish) */

int plainq_ins(plainq *qhp,plainq_ent *ep) noex {
	int		rs ;
	int		rc = 0 ;
	if ((rs = plainq_magic(qhp,ep)) >= 0) {
		const ca	qha = ca(qhp) ;
		if (qhp->head && qhp->tail) {
		    plainq_ent	*tep = entp(qhp->tail + qha) ;
	            tep->next = (ca(ep) - qha) ;
	            ep->prev = (ca(tep) - qha) ;
		} else if (qhp->head || qhp->tail) {
		    rs = SR_BADFMT ;
		} else {
	            qhp->head = (ca(ep) - qha) ;
	            ep->prev = 0 ;
	        } /* end if */
		if (rs >= 0) {
	            ep->next = 0 ;
	            qhp->tail = (ca(ep) - qha) ;
	            rc = ++qhp->count ;
		}
	} /* end if (magic) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (plainq_ins) */

int plainq_insgroup(plainq *qhp,plainq_ent *gp,int esize,int n) noex {
	int		rs ;
	int		rc = 0 ;
	if ((rs = plainq_magic(qhp,gp)) >= 0) {
		rs = SR_INVALID ;
		if ((n > 0) && (esize > 0)) {
	            plainq_ent	*ep ;
	            caddr_t	p = (caddr_t) gp ;
		    rs = SR_OK ;
	            for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
	                ep = (plainq_ent *) p ;
	                rs = plainq_ins(qhp,ep) ;
	                p += esize ;
	            } /* end for */
	            rc = qhp->count ;
		} else if (n == 0) {
		    rs = SR_OK ;
		}
	} /* end if (magic) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (plainq_insgroup) */

int plainq_inshead(plainq *qhp,plainq_ent *ep) noex {
	int		rs ;
	int		rc = 0 ;
	if ((rs = plainq_magic(qhp,ep)) >= 0) {
		const ca	qha = ca(qhp) ;
		if (qhp->head && qhp->tail) {
		    plainq_ent	*hep = entp(qhp->head + qha) ;
	            qhp->head = (ca(ep) - qha) ;
	            hep->prev = (ca(ep) - qha) ;
	            ep->next = (ca(hep) - qha) ;
	            ep->prev = 0 ;
		} else if (qhp->head || qhp->tail) {
		    rs = SR_BADFMT ;
		} else {
	            qhp->head = (ca(ep) - qha) ;
	            qhp->tail = (ca(ep) - qha) ;
	            ep->next = 0 ;
	            ep->prev = 0 ;
	        } /* end if */
		if (rs >= 0) {
	            rc = ++qhp->count ;
		}
	} /* end if (magic) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (plainq_inshead) */

int plainq_unlink(plainq *qhp,plainq_ent *ep) noex {
	int		rs ;
	int		rc = 0 ;
	if ((rs = plainq_magic(qhp,ep)) >= 0) {
		const ca	qha = ca(qhp) ;
		rs = SR_EMPTY ;
		if (qhp->head && qhp->tail) {
		    plainq_ent	*nep, *pep ;
		    rs = SR_OK ;
	            if (ep->next != 0) {
	                nep = entp(qha + ep->next) ;
	                if (ep->prev != 0) {
	                    nep->prev = ep->prev ;
		            pep = entp(qha + ep->prev) ;
		            pep->next = ep->next ;
	                } else {
		            ptrdiff_t	eo = (ca(ep) - qha) ;
		            if (qhp->head == eo) {
	                        nep->prev = ep->prev ;
	                        qhp->head = (ca(nep) - qha) ;
		            } else {
		                rs = SR_BADFMT ;
		            }
	                }
	            } else {
	                ptrdiff_t	eo = (ca(ep) - qha) ;
	                if (ep->prev != 0) {
		            pep = entp(qha + ep->prev) ;
		            if (qhp->tail == eo) {
		                pep->next = 0 ;
	                        qhp->tail =  (ca(pep) - qha) ;
		            } else {
		                rs = SR_BADFMT ;
		            }
	                } else {
		            if ((qhp->head == eo) && (qhp->tail == eo)) {
	                        qhp->head =  0 ;
	                        qhp->tail =  0 ;
		            } else {
		                rs = SR_BADFMT ;
		            }
	                }
	            } /* end if */
	            if (rs >= 0) {
	                ep->next = 0 ;
	                ep->prev = 0 ;
	                rc = --qhp->count ;
	            }
		} else {
		    if (qhp->head || qhp->tail) rs = SR_BADFMT ;
		}
	} /* end if (magic) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (plainq_unlink) */

int plainq_rem(plainq *qhp,plainq_ent **epp) noex {
	int		rs ;
	int		rc = 0 ;
	if ((rs = plainq_magic(qhp)) >= 0) {
		const ca	qha = ca(qhp) ;
		rs = SR_EMPTY ;
	        if (qhp->head && qhp->tail) {
                    plainq_ent       *ep = entp(qhp->head + qha) ;
                    if (qhp->head != qhp->tail) {
			rs = SR_BADFMT ;
			if (ep->next && (ep->prev == 0)) {
                            plainq_ent   *nep = entp(ep->next + qha) ;
			    rs = SR_OK ;
                            nep->prev = 0 ;
                            qhp->head = ep->next ;
                            rc = --qhp->count ;
			}
                    } else {
			rs = SR_OK ;
                        qhp->head = 0 ;
                        qhp->tail = 0 ;
                        rc = --qhp->count ;
                    }
                    if (epp && (rs >= 0)) {
                        ep->prev = 0 ;
                        ep->next = 0 ;
                        *epp = ep ;
                    }
		} else {
		    if (qhp->head || qhp->tail) rs = SR_BADFMT ;
		} /* end if (not-empty) */
		if (epp && (rs < 0)) *epp = nullptr ;
	} /* end if (magic) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (plainq_rem) */

int plainq_gethead(plainq *qhp,plainq_ent **epp) noex {
	int		rs ;
	int		rc = 0 ;
	if ((rs = plainq_magic(qhp,epp)) >= 0) {
		const ca	qha = ca(qhp) ;
		rs = SR_EMPTY ;
	        *epp = nullptr ;
	        if (qhp->head && qhp->tail) {
	            plainq_ent	*ep = entp(qha + qhp->head) ;
	            if (ep->prev == 0) {
			rs = SR_OK ;
	                *epp = ep ;
		        rc = qhp->count ;
	            } else {
		        rs = SR_BADFMT ;
	            }
		} else {
		    if (qhp->head || qhp->tail) rs = SR_BADFMT ;
		} /* end if (not-empty) */
	} /* end if (magic) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (plainq_gethead) */

int plainq_remtail(plainq *qhp,plainq_ent **epp) noex {
	int		rs ;
	int		rc = 0 ;
	if ((rs = plainq_magic(qhp)) >= 0) {
		const ca	qha = ca(qhp) ;
		rs = SR_EMPTY ;
	        if (qhp->head && qhp->tail) {
	            plainq_ent	*ep = entp(qha + qhp->tail) ;
		    if (qhp->head != qhp->tail) {
		        rs = SR_BADFMT ;
	                if ((ep->next == 0) && ep->prev) {
	                    plainq_ent	*pep = entp(qha + ep->prev) ;
			    rs = SR_OK ;
	                    qhp->tail = ep->prev ;
	                    if (qhp->tail == 0) qhp->head = 0 ;
			    pep->next = 0 ;
		            rc = --qhp->count ;
			}
		    } else {
			rs = SR_OK ;
			qhp->head = 0 ;
			qhp->tail = 0 ;
			rc = --qhp->count ;
		    }
	            if (epp) {
			ep->next = 0 ;
			ep->prev = 0 ;
			*epp = ep ;
		    }
		} else {
		    if (qhp->head || qhp->tail) rs = SR_BADFMT ;
		} /* end if (not-empty) */
		if (epp && (rs < 0)) *epp = nullptr ;
	} /* end if (magic) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (plainq_remtail) */

int plainq_gettail(plainq *qhp,plainq_ent **epp) noex {
	int		rs ;
	int		rc = 0 ;
	if ((rs = plainq_magic(qhp,epp)) >= 0) {
		const ca	qha = ca(qhp) ;
		rs = SR_EMPTY ;
	        *epp = nullptr ;
	        if (qhp->head && qhp->tail) {
	            plainq_ent	*ep = entp(qha + qhp->tail) ;
	            if (ep->next == 0) {
			rs = SR_OK ;
		        *epp = ep ;
		        rc = qhp->count ;
	            } else {
		        rs = SR_BADFMT ;
	            }
		} else {
		    if (qhp->head || qhp->tail) rs = SR_BADFMT ;
		} /* end if (not-empty) */
	} /* end if (magic) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (plainq_gettail) */

int plainq_count(plainq *qhp) noex {
	int		rs ;
	if ((rs = plainq_magic(qhp)) >= 0) {
		rs = qhp->count ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (plainq_count) */

int plainq_audit(plainq *qhp) noex {
	int		rs ;
	int		rc = 0 ;
	if ((rs = plainq_magic(qhp)) >= 0) {
		const ca	qha = ca(qhp) ;
	        if (qhp->head && qhp->tail) {
	            int		n = 0 ;
	            ptrdiff_t	next = qhp->head ;
	            while (next != 0) {
		        plainq_ent	*ep = entp(qha + next) ;
		        n += 1 ;
	                next = ep->next ;
	            } /* end while */
		    rc = qhp->count ;
	            if (qhp->count != n) {
	                rs = SR_BADFMT ;
	            }
		} else {
		    if (qhp->head || qhp->tail) {
			rs = SR_BADFMT ;
		    } else if (qhp->count != 0) {
			rs = SR_BADFMT ;
		    }
	        }
	} /* end if (magic) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (plainq_audit) */



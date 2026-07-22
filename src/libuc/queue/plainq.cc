/* plainq SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

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
#include	<usysbase.h>
#include	<ulogerror.h>
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
local int plainq_magic(plainq *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == PLAINQ_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (plainq_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int plainq_start(plainq *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    op->head = 0 ;
	    op->tail = 0 ;
	    op->cnt = 0 ;
	    op->magval = PLAINQ_MAGIC ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (plainq_start) */

int plainq_finish(plainq *op) noex {
	int		rs ;
	if ((rs = plainq_magic(op)) >= 0) ylikely {
	    op->head = 0 ;
	    op->tail = 0 ;
	    op->cnt = 0 ;
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (plainq_finish) */

int plainq_ins(plainq *op,plainq_ent *ep) noex {
	int		rs ;
	int		rc = 0 ;
	if ((rs = plainq_magic(op,ep)) >= 0) ylikely {
	    const ca	qha = ca(op) ;
	    if (op->head && op->tail) {
		plainq_ent	*tep = entp(op->tail + qha) ;
	        tep->next = (ca(ep) - qha) ;
	        ep->prev = (ca(tep) - qha) ;
	    } else if (op->head || op->tail) {
		rs = SR_BADFMT ;
	    } else {
	        op->head = (ca(ep) - qha) ;
	        ep->prev = 0 ;
	    } /* end if */
	    if (rs >= 0) ylikely {
	        ep->next = 0 ;
	        op->tail = (ca(ep) - qha) ;
	        rc = ++op->cnt ;
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (plainq_ins) */

int plainq_insgroup(plainq *op,plainq_ent *gp,int esize,int n) noex {
	int		rs ;
	int		rc = 0 ;
	if ((rs = plainq_magic(op,gp)) >= 0) ylikely {
		rs = SR_INVALID ;
		if ((n > 0) && (esize > 0)) {
	            plainq_ent	*ep ;
	            caddr_t	p = (caddr_t) gp ;
		    rs = SR_OK ;
	            for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
	                ep = (plainq_ent *) p ;
	                rs = plainq_ins(op,ep) ;
	                p += esize ;
	            } /* end for */
	            rc = op->cnt ;
		} else if (n == 0) {
		    rs = SR_OK ;
		}
	} /* end if (magic) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (plainq_insgroup) */

int plainq_inshead(plainq *op,plainq_ent *ep) noex {
	int		rs ;
	int		rc = 0 ;
	if ((rs = plainq_magic(op,ep)) >= 0) ylikely {
		const ca	qha = ca(op) ;
		if (op->head && op->tail) {
		    plainq_ent	*hep = entp(op->head + qha) ;
	            op->head = (ca(ep) - qha) ;
	            hep->prev = (ca(ep) - qha) ;
	            ep->next = (ca(hep) - qha) ;
	            ep->prev = 0 ;
		} else if (op->head || op->tail) {
		    rs = SR_BADFMT ;
		} else {
	            op->head = (ca(ep) - qha) ;
	            op->tail = (ca(ep) - qha) ;
	            ep->next = 0 ;
	            ep->prev = 0 ;
	        } /* end if */
		if (rs >= 0) ylikely {
	            rc = ++op->cnt ;
		}
	} /* end if (magic) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (plainq_inshead) */

int plainq_unlink(plainq *op,plainq_ent *ep) noex {
	int		rs ;
	int		rc = 0 ;
	if ((rs = plainq_magic(op,ep)) >= 0) ylikely {
		const ca	qha = ca(op) ;
		rs = SR_EMPTY ;
		if (op->head && op->tail) {
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
		            if (op->head == eo) {
	                        nep->prev = ep->prev ;
	                        op->head = (ca(nep) - qha) ;
		            } else {
		                rs = SR_BADFMT ;
		            }
	                }
	            } else {
	                ptrdiff_t	eo = (ca(ep) - qha) ;
	                if (ep->prev != 0) {
		            pep = entp(qha + ep->prev) ;
		            if (op->tail == eo) {
		                pep->next = 0 ;
	                        op->tail =  (ca(pep) - qha) ;
		            } else {
		                rs = SR_BADFMT ;
		            }
	                } else {
		            if ((op->head == eo) && (op->tail == eo)) {
	                        op->head =  0 ;
	                        op->tail =  0 ;
		            } else {
		                rs = SR_BADFMT ;
		            }
	                }
	            } /* end if */
	            if (rs >= 0) ylikely {
	                ep->next = 0 ;
	                ep->prev = 0 ;
	                rc = --op->cnt ;
	            }
		} else {
		    if (op->head || op->tail) rs = SR_BADFMT ;
		}
	} /* end if (magic) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (plainq_unlink) */

int plainq_rem(plainq *op,plainq_ent **epp) noex {
	int		rs ;
	int		rc = 0 ;
	if ((rs = plainq_magic(op)) >= 0) ylikely {
		const ca	qha = ca(op) ;
		rs = SR_EMPTY ;
	        if (op->head && op->tail) {
                    plainq_ent       *ep = entp(op->head + qha) ;
                    if (op->head != op->tail) {
			rs = SR_BADFMT ;
			if (ep->next && (ep->prev == 0)) {
                            plainq_ent   *nep = entp(ep->next + qha) ;
			    rs = SR_OK ;
                            nep->prev = 0 ;
                            op->head = ep->next ;
                            rc = --op->cnt ;
			}
                    } else {
			rs = SR_OK ;
                        op->head = 0 ;
                        op->tail = 0 ;
                        rc = --op->cnt ;
                    }
                    if (epp && (rs >= 0)) {
                        ep->prev = 0 ;
                        ep->next = 0 ;
                        *epp = ep ;
                    }
		} else {
		    if (op->head || op->tail) rs = SR_BADFMT ;
		} /* end if (not-empty) */
		if (epp && (rs < 0)) *epp = nullptr ;
	} /* end if (magic) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (plainq_rem) */

int plainq_gethead(plainq *op,plainq_ent **epp) noex {
	int		rs ;
	int		rc = 0 ;
	if ((rs = plainq_magic(op,epp)) >= 0) ylikely {
		const ca	qha = ca(op) ;
		rs = SR_EMPTY ;
	        *epp = nullptr ;
	        if (op->head && op->tail) {
	            plainq_ent	*ep = entp(qha + op->head) ;
	            if (ep->prev == 0) {
			rs = SR_OK ;
	                *epp = ep ;
		        rc = op->cnt ;
	            } else {
		        rs = SR_BADFMT ;
	            }
		} else {
		    if (op->head || op->tail) rs = SR_BADFMT ;
		} /* end if (not-empty) */
	} /* end if (magic) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (plainq_gethead) */

int plainq_remtail(plainq *op,plainq_ent **epp) noex {
	int		rs ;
	int		rc = 0 ;
	if ((rs = plainq_magic(op)) >= 0) ylikely {
		const ca	qha = ca(op) ;
		rs = SR_EMPTY ;
	        if (op->head && op->tail) {
	            plainq_ent	*ep = entp(qha + op->tail) ;
		    if (op->head != op->tail) {
		        rs = SR_BADFMT ;
	                if ((ep->next == 0) && ep->prev) {
	                    plainq_ent	*pep = entp(qha + ep->prev) ;
			    rs = SR_OK ;
	                    op->tail = ep->prev ;
	                    if (op->tail == 0) op->head = 0 ;
			    pep->next = 0 ;
		            rc = --op->cnt ;
			}
		    } else {
			rs = SR_OK ;
			op->head = 0 ;
			op->tail = 0 ;
			rc = --op->cnt ;
		    }
	            if (epp) {
			ep->next = 0 ;
			ep->prev = 0 ;
			*epp = ep ;
		    }
		} else {
		    if (op->head || op->tail) rs = SR_BADFMT ;
		} /* end if (not-empty) */
		if (epp && (rs < 0)) *epp = nullptr ;
	} /* end if (magic) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (plainq_remtail) */

int plainq_gettail(plainq *op,plainq_ent **epp) noex {
	int		rs ;
	int		rc = 0 ;
	if ((rs = plainq_magic(op,epp)) >= 0) ylikely {
		const ca	qha = ca(op) ;
		rs = SR_EMPTY ;
	        *epp = nullptr ;
	        if (op->head && op->tail) {
	            plainq_ent	*ep = entp(qha + op->tail) ;
	            if (ep->next == 0) {
			rs = SR_OK ;
		        *epp = ep ;
		        rc = op->cnt ;
	            } else {
		        rs = SR_BADFMT ;
	            }
		} else {
		    if (op->head || op->tail) rs = SR_BADFMT ;
		} /* end if (not-empty) */
	} /* end if (magic) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (plainq_gettail) */

int plainq_count(plainq *op) noex {
	int		rs ;
	if ((rs = plainq_magic(op)) >= 0) ylikely {
		rs = op->cnt ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (plainq_count) */

int plainq_audit(plainq *op) noex {
	int		rs ;
	int		rc = 0 ;
	if ((rs = plainq_magic(op)) >= 0) ylikely {
		const ca	qha = ca(op) ;
	        if (op->head && op->tail) {
	            int		n = 0 ;
	            ptrdiff_t	next = op->head ;
	            while (next != 0) {
		        plainq_ent	*ep = entp(qha + next) ;
		        n += 1 ;
	                next = ep->next ;
	            } /* end while */
		    rc = op->cnt ;
	            if (op->cnt != n) {
	                rs = SR_BADFMT ;
	            }
		} else {
		    if (op->head || op->tail) {
			rs = SR_BADFMT ;
		    } else if (op->cnt != 0) {
			rs = SR_BADFMT ;
		    }
	        }
	} /* end if (magic) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (plainq_audit) */

int plainq::ins(plainq_ent *ep) noex {
	return plainq_ins(this,ep) ;
}

int plainq::insgroup(plainq_ent *ep,int esz,int n) noex {
	return plainq_insgroup(this,ep,esz,n) ;
}

int plainq::inshead(plainq_ent *ep) noex {
	return plainq_inshead(this,ep) ;
}

int plainq::gethead(plainq_ent **rpp) noex {
	return plainq_gethead(this,rpp) ;
}

int plainq::gettail(plainq_ent **rpp) noex {
	return plainq_gettail(this,rpp) ;
}

int plainq::rem(plainq_ent **rpp) noex {
	return plainq_rem(this,rpp) ;
}

int plainq::remtail(plainq_ent **rpp) noex {
	return plainq_remtail(this,rpp) ;
}

int plainq::unlink(plainq_ent *ep) noex {
	return plainq_unlink(this,ep) ;
}

void plainq::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("plainq",rs,"fini-finish") ;
	}
} /* end method (plainq::dtor) */

plainq::operator int () noex {
	return plainq_count(this) ;
} /* end method (plainq::operator) */

plainq_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case plainqmem_start:
	        rs = plainq_start(op) ;
	        break ;
	    case plainqmem_count:
	        rs = plainq_count(op) ;
	        break ;
	    case plainqmem_audit:
	        rs = plainq_audit(op) ;
	        break ;
	    case plainqmem_finish:
	        rs = plainq_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (plainq_co::operator) */




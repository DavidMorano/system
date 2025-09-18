/* slq SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Single-list-Queue - regular (no-frills) single list pointer queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was modeled after assembly.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	slq

	Description:
	This is a regular, pointer based, no-frills sigle list queue.  

*******************************************************************************/

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

#include	"slq.h"


/* local defines */


/* local namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


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

int slq_gethead(slq *op,slq_ent **epp) noex {
	int		rs = SR_FAULT ;
	if (op && epp) {
	    rs = SR_OK ;
	    if (op->head && op->tail) {
	        *epp = op->head ;
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
/* end subroutine (slq_gethead) */

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


/* local subroutines */

void slq::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("slq",rs,"fini-finish") ;
	}
} /* end method (slq::dtor) */

slq_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case slqmem_start:
	        rs = slq_start(op) ;
	        break ;
	    case slqmem_audit:
	        rs = slq_audit(op) ;
	        break ;
	    case slqmem_finish:
	        rs = slq_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (slq_co::operator) */

int slq::ins(slq_ent *ep) noex {
	return slq_ins(this,ep) ;
}

int slq::insgroup(slq_ent *gp,int esz,int n) noex {
	return slq_insgroup(this,gp,esz,n) ;
}

int slq::rem(slq_ent **epp) noex {
	return slq_rem(this,epp) ;
}

int slq::gethead(slq_ent **epp) noex {
	return slq_gethead(this,epp) ;
}

int slq::gettail(slq_ent **epp) noex {
	return slq_gettail(this,epp) ;
}



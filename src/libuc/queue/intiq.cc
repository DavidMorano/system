/* intiq SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Integer-Interlocked Queue management */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-11-24, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	intiq

	Description:
	This object manages interlocked FIFO-integer operations.

	Notes:
	+ thread-safe

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
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

#include	"intiq.h"

import libutil ;

/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int intiq_ctor(intiq *op,Args ... args) noex {
    	INTIQ		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->mxp = new(nothrow) ptm) != np) {
	        if ((op->fqp = new(nothrow) fifoitem) != np) {
		    rs = SR_OK ;
	        } /* end if (new-fifoitem) */
	    } /* end if (new-ptm) */
	    if (rs < 0) {
		delete op->mxp ;
		op->mxp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (intiq_ctor) */

static int intiq_dtor(intiq *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->fqp) {
		delete op->fqp ;
		op->fqp = nullptr ;
	    }
	    if (op->mxp) {
		delete op->mxp ;
		op->mxp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (intiq_dtor) */

template<typename ... Args>
static inline int intiq_magic(intiq *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == INTIQ_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (intiq_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int intiq_start(intiq *op) noex {
	int		rs ;
	if ((rs = intiq_ctor(op)) >= 0) {
	    if ((rs = ptm_create(op->mxp,nullptr)) >= 0) {
	        if ((rs = fifoitem_start(op->fqp)) >= 0) {
		    op->magic = INTIQ_MAGIC ;
		}
	        if (rs < 0) {
		    ptm_destroy(op->mxp) ;
		}
	    }
	    if (rs < 0) {
		intiq_dtor(op) ;
	    }
	} /* end if (intiq_ctor) */
	return rs ;
}
/* end subroutine (intiq_start) */

int intiq_finish(intiq *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = intiq_magic(op)) >= 0) {
	    if (op->fqp) {
	        rs1 = fifoitem_finish(op->fqp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->mxp) {
	        rs1 = ptm_destroy(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = intiq_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (intiq_finish) */

int intiq_ins(intiq *op,int ch) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = intiq_magic(op)) >= 0) {
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
	        {
	            cint	esz = szof(int) ;
	            rs = fifoitem_ins(op->fqp,&ch,esz) ;
		    c = rs ;
	        }
	        rs1 = ptm_unlock(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (intiq_ins) */

int intiq_rem(intiq *op,int *chp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = intiq_magic(op)) >= 0) {
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
	        {
		    cint	esz = szof(int) ;
	            rs = fifoitem_rem(op->fqp,chp,esz) ;
	            c = rs ;
	        }
	        rs1 = ptm_unlock(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (intiq_rem) */

int intiq_count(intiq *op) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = intiq_magic(op)) >= 0) {
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
	        {
	            rs = fifoitem_count(op->fqp) ;
		    c = rs ;
	        }
	        rs1 = ptm_unlock(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (intiq_count) */



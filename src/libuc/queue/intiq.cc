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

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ulogerror.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"intiq.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

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
local int intiq_ctor(intiq *op,Args ... args) noex {
    	INTIQ		*hop = op ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->mxp = new(nothrow) ptm) != np) ylikely {
	        if ((op->fqp = new(nothrow) fifoitem) != np) ylikely {
		    rs = SR_OK ;
	        } /* end if (new-fifoitem) */
	    } /* end if (new-ptm) */
	    if (rs < 0) {
		delete op->mxp ;
		op->mxp = nullptr ;
	    } /* end if (error) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (intiq_ctor) */

local int intiq_dtor(intiq *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->fqp) ylikely {
		delete op->fqp ;
		op->fqp = nullptr ;
	    }
	    if (op->mxp) ylikely {
		delete op->mxp ;
		op->mxp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (intiq_dtor) */

template<typename ... Args>
local inline int intiq_magic(intiq *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == INTIQ_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (intiq_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int intiq_start(intiq *op) noex {
	int		rs ;
	if ((rs = intiq_ctor(op)) >= 0) ylikely {
	    ptm *mxp = op->mxp ;
	    if ((rs = mxp->create) >= 0) ylikely {
	        if ((rs = fifoitem_start(op->fqp)) >= 0) ylikely {
		    op->magval = INTIQ_MAGIC ;
		}
	        if (rs < 0) {
		    mxp->destroy() ;
		} /* end if (error) */
	    } /* end if */
	    if (rs < 0) {
		intiq_dtor(op) ;
	    } /* end if (error) */
	} /* end if (intiq_ctor) */
	return rs ;
} /* end subroutine (intiq_start) */

int intiq_finish(intiq *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = intiq_magic(op)) >= 0) ylikely {
	    if (op->fqp) ylikely {
	        rs1 = fifoitem_finish(op->fqp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->mxp) ylikely {
	        ptm *mxp = op->mxp ;
	        rs1 = mxp->destroy ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = intiq_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (intiq_finish) */

int intiq_ins(intiq *op,int ch) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = intiq_magic(op)) >= 0) ylikely {
	    ptm *mxp = op->mxp ;
	    if ((rs = mxp->lockbegin) >= 0) ylikely {
	        {
	            cint	esz = szof(int) ;
	            rs = fifoitem_ins(op->fqp,&ch,esz) ;
		    c = rs ;
	        }
	        rs1 = mxp->lockend ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (intiq_ins) */

int intiq_rem(intiq *op,int *chp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = intiq_magic(op)) >= 0) ylikely {
	    ptm *mxp = op->mxp ;
	    if ((rs = mxp->lockbegin) >= 0) ylikely {
	        {
		    cint	esz = szof(int) ;
	            rs = fifoitem_rem(op->fqp,chp,esz) ;
	            c = rs ;
	        }
	        rs1 = mxp->lockend ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (intiq_rem) */

int intiq_count(intiq *op) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = intiq_magic(op)) >= 0) ylikely {
	    ptm *mxp = op->mxp ;
	    if ((rs = mxp->lockbegin) >= 0) ylikely {
	        {
	            rs = fifoitem_count(op->fqp) ;
		    c = rs ;
	        }
	        rs1 = mxp->lockend ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (intiq_count) */

int intiq::ins(int ch) noex {
	return intiq_ins(this,ch) ;
} /* end method */

int intiq::rem(int *rp) noex {
	return intiq_rem(this,rp) ;
} /* end method */

void intiq::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("intiq",rs,"fini-finish") ;
	}
} /* end method (intiq::dtor) */

intiq::operator int () noex {
        return intiq_count(this) ;
} /* end method (intiq::operator) */

intiq_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case intiqmem_start:
	        rs = intiq_start(op) ;
	        break ;
	    case intiqmem_count:
	        rs = intiq_count(op) ;
	        break ;
	    case intiqmem_finish:
	        rs = intiq_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (intiq_co::operator) */



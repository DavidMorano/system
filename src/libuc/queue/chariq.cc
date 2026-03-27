/* chariq SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Character-Interlocked Queue management */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1994 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	chariq

	Description:
	This object manages interlocked FIFO-character operations.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<ulogerror.h>
#include	<uclibmem.h>
#include	<charq.h>
#include	<ptm.h>
#include	<localmisc.h>

#include	"chariq.h"


/* local defines */

#define	DEFLEN		10


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int chariq_ctor(chariq *op,Args ... args) noex {
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    if ((op->cqp = new(nt) charq) != np) ylikely {
	        if ((op->mxp = new(nt) ptm) != np) ylikely {
		    rs = SR_OK ;
		} /* end if (new-ptm) */
		if (rs < 0) {
		    delete op->cqp ;
		    op->cqp = nullptr ;
		}
	    } /* end if (new-charq) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (chariq_ctor) */

local int chariq_dtor(chariq *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->mxp) {
		delete op->mxp ;
		op->mxp = nullptr ;
	    }
	    if (op->cqp) {
		delete op->cqp ;
		op->cqp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (chariq_dtor) */


/* local variables */


/* exported variables */


/* exported subroutines */

int chariq_start(chariq *op,int sz) noex {
	int		rs ;
	if ((rs = chariq_ctor(op)) >= 0) ylikely {
	    ptm *mxp = op->mxp ;
	    if (sz <= 0) sz = DEFLEN ;
	    if ((rs = mxp->create) >= 0) ylikely {
	        rs = charq_start(op->cqp,sz) ;
	        if (rs < 0) {
		    mxp->destroy() ;
	        }
	    } /* end if (ptm_create) */
	} /* end if (chariq_ctor) */
	return rs ;
}
/* end subroutine (chariq_start) */

int chariq_finish(chariq *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->cqp) ylikely {
	        rs1 = charq_finish(op->cqp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->mxp) ylikely {
	        ptm *mxp = op->mxp ;
	        rs1 = mxp->destroy ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = chariq_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (chariq_finish) */

int chariq_ins(chariq *op,int ch) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (op) ylikely {
	    ptm *mxp = op->mxp ;
	    if ((rs = mxp->lockbegin) >= 0) ylikely {
	        {
	            rs = charq_ins(op->cqp,ch) ;
		    c = rs ;
	        }
	        rs1 = mxp->lockend ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (chariq_ins) */

int chariq_rem(chariq *op,char *chp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (op) ylikely {
	    ptm *mxp = op->mxp ;
	    if ((rs = mxp->lockbegin) >= 0) ylikely {
	        {
	            rs = charq_rem(op->cqp,chp) ;
		    c = rs ;
	        }
	        rs1 = mxp->lockend ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (chariq_rem) */

int chariq_remall(chariq *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if (op) ylikely {
	    ptm *mxp = op->mxp ;
	    if ((rs = mxp->lockbegin) >= 0) ylikely {
		{
		    rs = charq_remall(op->cqp) ;
		    rv = rs ;
		}
	        rs1 = mxp->lockend ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (non-null) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (chariq_remall) */

int chariq_size(chariq *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if (op) ylikely {
	    ptm *mxp = op->mxp ;
	    if ((rs = mxp->lockbegin) >= 0) ylikely {
	        {
	            rs = charq_size(op->cqp) ;
		    rv = rs ;
	        }
	        rs1 = mxp->lockend ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (non-null) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (chariq_size) */

int chariq_count(chariq *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (op) ylikely {
	    ptm *mxp = op->mxp ;
	    if ((rs = mxp->lockbegin) >= 0) ylikely {
	        {
	            rs = charq_count(op->cqp) ;
		    c = rs ;
	        }
	        rs1 = mxp->lockend ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (chariq_count) */

int chariq::start(int µsz) noex {
	return chariq_start(this,µsz) ;
}

int chariq::ins(int ch) noex {
	return chariq_ins(this,ch) ;
}

int chariq::rem(char *rp) noex {
	return chariq_rem(this,rp) ;
}

void chariq::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("chariq",rs,"fini-finish") ;
	}
} /* end method (chariq::dtor) */

chariq::operator int () noex {
    	return chariq_count(this) ;
} /* end method (chariq::operator) */

chariq_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case chariqmem_remall:
	        rs = chariq_remall(op) ;
	        break ;
	    case chariqmem_size:
	        rs = chariq_size(op) ;
	        break ;
	    case chariqmem_count:
	        rs = chariq_count(op) ;
	        break ;
	    case chariqmem_len:
	        rs = chariq_count(op) ;
	        break ;
	    case chariqmem_finish:
	        rs = chariq_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (chariq_co::operator) */



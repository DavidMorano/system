/* q SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* self-realtive double-linked queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	q

	This object implements a self relative queue for shared
	memory applications where the shared memory segments have
	different starting addresses in two or more address spaces.
	This does NOT use any pointers back to the queue-header,
	so therefore the object (header) CAN be moved without
	problems.

	+ self-relative
	+ relocatable head
	+ double-linked
	+ thread-safe

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ulogerror.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ptma.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"q.h"


/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef plainq_ent	*entp ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local inline int q_ctor(q *op,Args ... args) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    if ((op->mxp = new(nothrow) ptm) != np) ylikely {
	        if ((op->pqp = new(nothrow) plainq) != np) ylikely {
		    rs = SR_OK ;
	        } /* end if (new-plainq) */
		if (rs < 0) {
		    delete op->mxp ;
		    op->mxp = nullptr ;
	        }
	    } /* end if (new-ptm) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (q_ctor) */

local inline int q_dtor(q *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->pqp) ylikely {
		delete op->pqp ;
		op->pqp = nullptr ;
	    }
	    if (op->mxp) ylikely {
		delete op->mxp ;
		op->mxp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (q_dtor) */


/* local variables */


/* exported variables */


/* exported subroutines */

int q_start(q *op,int type) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = q_ctor(op)) >= 0) ylikely {
	    bool	f_mutex = false ;
	    bool	f_plainq = false ;
	    if (ptma ma ; (rs = ptma_create(&ma)) >= 0) ylikely {
	        int	matype = PTHREAD_PROCESS_PRIVATE ;
	        if (type > 0) matype = PTHREAD_PROCESS_SHARED ;
	        if ((rs = ptma_setpshared(&ma,matype)) >= 0) ylikely {
	            if ((rs = op->mxp->create(&ma)) >= 0) ylikely {
			f_mutex = true ;
			if ((rs = plainq_start(op->pqp)) >= 0) ylikely {
			    f_plainq = true ;
			}
		    }
	        }
	        rs1 = ptma_destroy(&ma) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex-attributes) */
	    if (rs < 0) {
		if (f_plainq) plainq_finish(op->pqp) ;
		if (f_mutex) {
		    ptm *mxp = op->mxp ;
		    mxp->destroy() ;
		}
	    }
	    if (rs < 0) {
		q_dtor(op) ;
	    }
	} /* end if (q_ctor) */
	return rs ;
} /* end subroutine (q_start) */

int q_finish(q *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->pqp) ylikely {
	        rs1 = plainq_finish(op->pqp) ;
	        if (rs >= 0) rs = rs1 ;
	        rc = rs1 ;
	    }
	    if (op->mxp) ylikely {
		ptm *mxp = op->mxp ;
	        rs1 = mxp->destroy ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = q_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
} /* end subroutine (q_finish) */

int q_ins(q *op,Q_ENT *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (op && ep) ylikely {
	    if ((rs = op->mxp->lockbegin) >= 0) ylikely {
		{
		    entp	qep = entp(ep) ;
		    rs = plainq_ins(op->pqp,qep) ;
		    rc = rs ;
		}
	        rs1 = op->mxp->lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex lock) */
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
} /* end subroutine (q_ins) */

int q_inshead(q *op,Q_ENT *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (op && ep) ylikely {
	    if ((rs = op->mxp->lockbegin) >= 0) ylikely {
		{
		    entp	qep = entp(ep) ;
		    rs = plainq_inshead(op->pqp,qep) ;
		    rc = rs ;
		}
	        rs1 = op->mxp->lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex lock) */
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
} /* end subroutine (q_inshead) */

int q_rem(q *op,Q_ENT **epp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (op) ylikely {
	    if ((rs = op->mxp->lockbegin) >= 0) ylikely {
		{
		    entp	*qepp = (entp *) epp ;
		    rs = plainq_rem(op->pqp,qepp) ;
		    rc = rs ;
		}
	        rs1 = op->mxp->lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex lock) */
	    if (epp && (rs < 0)) *epp = nullptr ;
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
} /* end subroutine (q_rem) */

int q_remtail(q *op,Q_ENT **epp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (op) ylikely {
	    if ((rs = op->mxp->lockbegin) >= 0) ylikely {
		{
		    entp	*qepp = (entp *) epp ;
		    rs = plainq_remtail(op->pqp,qepp) ;
		    rc = rs ;
		}
	        rs1 = op->mxp->lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex lock) */
	    if (epp && (rs < 0)) *epp = nullptr ;
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
} /* end subroutine (q_remtail) */

int q_count(q *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (op) ylikely {
	    if ((rs = op->mxp->lockbegin) >= 0) ylikely {
		{
		    rs = plainq_count(op->pqp) ;
		    rc = rs ;
		}
	        rs1 = op->mxp->lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex lock) */
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
} /* end subroutine (q_count) */

int q::ins(q_ent *ep) noex {
	return q_ins(this,ep) ;
} /* end method */

int q::inshead(q_ent *ep) noex {
	return q_inshead(this,ep) ;
} /* end method */

int q::rem(q_ent **rpp) noex {
	return q_rem(this,rpp) ;
} /* end method */

int q::remtail(q_ent **rpp) noex {
	return q_remtail(this,rpp) ;
} /* end method */

void q::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("q",rs,"fini-finish") ;
	}
} /* end method (q::dtor) */

q::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (mxp) ylikely {
	    rs = q_count(this) ;
	} /* end if (non-null) */
	return rs ;
} /* end method (q::operator) */

int q_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case qmem_start:
	        rs = q_start(op,a) ;
	        break ;
	    case qmem_count:
	        rs = q_count(op) ;
	        break ;
	    case qmem_finish:
	        rs = q_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (q_co::operator) */



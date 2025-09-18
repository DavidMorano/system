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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<ptma.h>
#include	<localmisc.h>

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
static inline int q_ctor(q *op,Args ... args) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    if ((op->mxp = new(nothrow) ptm) != np) {
	        if ((op->pqp = new(nothrow) plainq) != np) {
		    rs = SR_OK ;
	        } /* end if (new-plainq) */
		if (rs < 0) {
		    delete op->mxp ;
		    op->mxp = nullptr ;
	        }
	    } /* end if (new-ptm) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (q_ctor) */

static inline int q_dtor(q *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->pqp) {
		delete op->pqp ;
		op->pqp = nullptr ;
	    }
	    if (op->mxp) {
		delete op->mxp ;
		op->mxp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (q_dtor) */


/* local variables */


/* exported variables */


/* exported subroutines */

int q_start(Q *op,int type) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = q_ctor(op)) >= 0) {
	    bool	f_mutex = false ;
	    bool	f_plainq = false ;
	    if (ptma ma ; (rs = ptma_create(&ma)) >= 0) {
	        int	matype = PTHREAD_PROCESS_PRIVATE ;
	        if (type > 0) matype = PTHREAD_PROCESS_SHARED ;
	        if ((rs = ptma_setpshared(&ma,matype)) >= 0) {
	            if ((rs = op->mxp->create(&ma)) >= 0) {
			f_mutex = true ;
			if ((rs = plainq_start(op->pqp)) >= 0) {
			    f_plainq = true ;
			}
		    }
	        }
	        rs1 = ptma_destroy(&ma) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex-attributes) */
	    if (rs < 0) {
		if (f_plainq) plainq_finish(op->pqp) ;
		if (f_mutex) op->mxp->destroy() ;
	    }
	    if (rs < 0) {
		q_dtor(op) ;
	    }
	} /* end if (q_ctor) */
	return rs ;
}
/* end subroutine (q_start) */

int q_finish(Q *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (op) {
	    rs = SR_OK ;
	    {
	        rs1 = plainq_finish(op->pqp) ;
	        if (rs >= 0) rs = rs1 ;
	        rc = rs1 ;
	    }
	    {
	        rs1 = op->mxp->destroy ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = q_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (q_finish) */

int q_ins(Q *op,Q_ENT *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (op && ep) {
	    if ((rs = op->mxp->lockbegin) >= 0) {
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
}
/* end subroutine (q_ins) */

int q_inshead(Q *op,Q_ENT *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (op && ep) {
	    if ((rs = op->mxp->lockbegin) >= 0) {
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
}
/* end subroutine (q_inshead) */

int q_rem(Q *op,Q_ENT **epp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (op) {
	    if ((rs = op->mxp->lockbegin) >= 0) {
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
}
/* end subroutine (q_rem) */

int q_remtail(Q *op,Q_ENT **epp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (op) {
	    if ((rs = op->mxp->lockbegin) >= 0) {
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
}
/* end subroutine (q_remtail) */

int q_count(Q *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (op) {
	    if ((rs = op->mxp->lockbegin) >= 0) {
		{
		    rs = plainq_count(op->pqp) ;
		    rc = rs ;
		}
	        rs1 = op->mxp->lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex lock) */
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (q_count) */



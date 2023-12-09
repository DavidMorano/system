/* q */
/* lang=C++20 */

/* self-realtive double-linked queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

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
#include	<usystem.h>
#include	<ptma.h>

#include	"q.h"


/* local defines */


/* local namespaces */


/* local typedefs */

typedef plainq_ent	*entp ;


/* exported subroutines */

int q_start(Q *qhp,int type) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (qhp) {
	    ptma	ma ;
	    bool	f_mutex = false ;
	    bool	f_plainq = false ;
	    if ((rs = ptma_create(&ma)) >= 0) {
	        int	matype = PTHREAD_PROCESS_PRIVATE ;
	        if (type > 0) matype = PTHREAD_PROCESS_SHARED ;
	        if ((rs = ptma_setpshared(&ma,matype)) >= 0) {
	            if ((rs = qhp->m.create(&ma)) >= 0) {
			f_mutex = true ;
			if ((rs = plainq_start(&qhp->pq)) >= 0) {
			    f_plainq = true ;
			}
		    }
	        }
	        rs1 = ptma_destroy(&ma) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex-attributes) */
	    if (rs < 0) {
		if (f_plainq) plainq_finish(&qhp->pq) ;
		if (f_mutex) qhp->m.destroy() ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (q_start) */

int q_finish(Q *qhp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (qhp) {
	    {
	        rs1 = plainq_finish(&qhp->pq) ;
	        if (rs >= 0) rs = rs1 ;
	        rc = rs1 ;
	    }
	    {
	        rs1 = qhp->m.destroy ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (q_finish) */

int q_ins(Q *qhp,Q_ENT *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (qhp && ep) {
	    if ((rs = qhp->m.lockbegin) >= 0) {
		{
		    entp	qep = entp(ep) ;
		    rs = plainq_ins(&qhp->pq,qep) ;
		    rc = rs ;
		}
	        rs1 = qhp->m.lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex lock) */
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (q_ins) */

int q_inshead(Q *qhp,Q_ENT *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (qhp && ep) {
	    if ((rs = qhp->m.lockbegin) >= 0) {
		{
		    entp	qep = entp(ep) ;
		    rs = plainq_inshead(&qhp->pq,qep) ;
		    rc = rs ;
		}
	        rs1 = qhp->m.lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex lock) */
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (q_inshead) */

int q_rem(Q *qhp,Q_ENT **epp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (qhp) {
	    if ((rs = qhp->m.lockbegin) >= 0) {
		{
		    entp	*qepp = (entp *) epp ;
		    rs = plainq_rem(&qhp->pq,qepp) ;
		    rc = rs ;
		}
	        rs1 = qhp->m.lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex lock) */
	    if (epp && (rs < 0)) *epp = nullptr ;
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (q_rem) */

int q_remtail(Q *qhp,Q_ENT **epp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (qhp) {
	    if ((rs = qhp->m.lockbegin) >= 0) {
		{
		    entp	*qepp = (entp *) epp ;
		    rs = plainq_remtail(&qhp->pq,qepp) ;
		    rc = rs ;
		}
	        rs1 = qhp->m.lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex lock) */
	    if (epp && (rs < 0)) *epp = nullptr ;
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (q_remtail) */

int q_count(Q *qhp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (qhp) {
	    if ((rs = qhp->m.lockbegin) >= 0) {
		{
		    rs = plainq_count(&qhp->pq) ;
		    rc = rs ;
		}
	        rs1 = qhp->m.lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex lock) */
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (q_count) */



/* aiq SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Asynchronous Interrupt Queue - some sort of queue object for OS stuff! */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This module was originally written.

	= 1998-07-01, David A­D­ Morano
	This module was enhanced by adding the POSIX thread mutex calls.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	This object implements a self-relative doublely linked list
	for queue operations. This type of queue can be used in
	shared memory area that are mapped to different addresses
	in different address spaces! That is the whole idea of this
	type of a queue. This queue is NOT circularly linked, so
	therefore the object (header) CAN be moved without problems.

	Important note:

	This type of queue (used for system OS or other executive
	type purposes) requires that all entries start with the
	structure 'aiq_ent'. This is necessary because that structure
	is used for the linking of the entries into the Q. This is
	not a container object in the normal sense in that access
	inside entries is required for operation! If you want a
	container Q that does not have to access the entry object
	then pick some other Q object to use instead of this one!

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<sigblocker.h>
#include	<localmisc.h>

#include	"aiq.h"


/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef q_ent		*entp ;


/* forward references */

template<typename ... Args>
static inline int aiq_ctor(aiq *op,Args ... args) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    if ((op->qp = new(nothrow) q) != np) {
		rs = SR_OK ;
	    } /* end if (new-pq) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (aiq_ctor) */

static inline int aiq_dtor(aiq *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->qp) {
		delete op->qp ;
		op->qp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (aiq_dtor) */

template<typename ... Args>
static int aiq_magic(aiq *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == AIQ_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}


/* local variables */


/* exported variables */


/* exported subroutines */

int aiq_start(aiq *op,int type) noex {
	int		rs ;
	if ((rs = aiq_ctor(op)) >= 0) {
	    if ((rs = q_start(op->qp,type)) >= 0) {
		op->magic = AIQ_MAGIC ;
	    } /* end if (q-start) */
	    if (rs < 0) {
		aiq_dtor(op) ;
	    }
	} /* end if (aiq_ctor) */
	return rs ;
}
/* end subroutine (aiq_start) */

int aiq_finish(aiq *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = aiq_magic(op)) >= 0) {
	    {
		rs1 = q_finish(op->qp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = aiq_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (aiq_finish) */

int aiq_ins(aiq *op,aiq_ent *ep) noex {
	int		rs ;
	int		rs1 ;
	int		rc = 0 ;
	if ((rs = aiq_magic(op,ep)) >= 0) {
	        sigblocker	b ;
	        if ((rs = b.start) >= 0) {
		    {
		        entp	qep = entp(ep) ;
		        rs = q_ins(op->qp,qep) ;
		        rc = rs ;
		    }
	            rs1 = b.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (sigblock) */
	} /* end if (magic) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (aiq_ins) */

int aiq_inshead(aiq *op,aiq_ent *ep) noex {
	int		rs ;
	int		rs1 ;
	int		rc = 0 ;
	if ((rs = aiq_magic(op,ep)) >= 0) {
	        sigblocker	b ;
	        if ((rs = b.start) >= 0) {
		    {
		        entp	qep = entp(ep) ;
		        rs = q_inshead(op->qp,qep) ;
		        rc = rs ;
		    }
	            rs1 = b.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (sigblock) */
	} /* end if (magic) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (aiq_inshead) */

int aiq_rem(aiq *op,aiq_ent **epp) noex {
	int		rs ;
	int		rs1 ;
	int		rc = 0 ;
	if ((rs = aiq_magic(op)) >= 0) {
	        sigblocker	b ;
	        if ((rs = b.start) >= 0) {
		    {
		        entp	*qepp = (entp *) epp ;
		        rs = q_rem(op->qp,qepp) ;
		        rc = rs ;
		    }
	            rs1 = b.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (sigblock) */
	} /* end if (magic) */
	if (epp && (rs < 0)) *epp = nullptr ;
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (aiq_rem) */

int aiq_remtail(aiq *op,aiq_ent **epp) noex {
	int		rs ;
	int		rs1 ;
	int		rc = 0 ;
	if ((rs = aiq_magic(op)) >= 0) {
	        sigblocker	b ;
	        if ((rs = b.start) >= 0) {
		    {
		        entp	*qepp = (entp *) epp ;
		        rs = q_remtail(op->qp,qepp) ;
		        rc = rs ;
		    }
	            rs1 = b.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (sigblock) */
	} /* end if (magic) */
	if (epp && (rs < 0)) *epp = nullptr ;
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (aiq_remtail) */

int aiq_count(aiq *op) noex {
	int		rs ;
	if ((rs = aiq_magic(op)) >= 0) {
	    rs = q_count(op->qp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (aiq_count) */



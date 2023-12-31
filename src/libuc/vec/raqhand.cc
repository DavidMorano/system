/* raqhand SUPPORT */
/* lang=C++20 */

/* random-access queue operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	These routines are used when the caller just wants to store
	their own pointer in a FIFO-ordered vector. These routines
	will not copy the structure pointed to by the passed pointer.
	The caller is responsible for keeping the original data in
	scope during the whole life span of the vector list.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<localmisc.h>

#include	"raqhand.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	raqhand_setopts(raqhand *,int) noex ;
static int	raqhand_valid(raqhand *,int) noex ;


/* local variables */


/* exported subroutines */

int raqhand_start(raqhand *op,int n,int opts) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if (n <= 1) n = RAQHAND_DEFENTS ;
	    memclear(op) ;
	    if ((rs = raqhand_setopts(op,opts)) >= 0) {
	        cint	size = ((n+1) * sizeof(void *)) ;
	        void	*vp{} ;
	        if ((rs = uc_libmalloc(size,&vp)) >= 0) {
		    memclear(vp,size) ;
		    op->va = (cvoid **) vp ;
	            op->n = n ;
	        } /* end if (memory-allocation) */
	    } /* end if (options) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (raqhand_start) */

int raqhand_finish(raqhand *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = SR_OK ;
		{
		    rs1 = uc_libfree(op->va) ;
		    if (rs >= 0) rs = rs1 ;
		    op->va = nullptr ;
		}
		op->c = 0 ;
		op->n = 0 ;
	    } /* end if (was open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (raqhand_finish) */

int raqhand_ins(raqhand *op,cvoid *ep) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && ep) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = SR_OVERFLOW ;	/* <- note "overflow" */
	        if (op->c < op->n) {
		    rs = SR_OK ;
	            i = op->ti ;
	            op->va[i] = ep ;
	            op->ti = ((op->ti+1)%op->n) ;
	            op->c += 1 ;
	        }
	    } /* end if (was open) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (raqhand_ins) */

int raqhand_acc(raqhand *op,int ai,void **rpp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = SR_OK ;
	        if (ai < op->c) {
	            i = ((ai+op->hi)%op->n) ;
	        } else {
	            rs = SR_NOTFOUND ;
	        }
	        if (rpp) {
		    void	*vp = (void *) op->va[i] ;
	            *rpp = (rs >= 0) ? vp : nullptr ;
	        }
	    } /* end if (was open) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (raqhand_acc) */

int raqhand_acclast(raqhand *op,void **rpp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = SR_NOTFOUND ;
	        if (op->c > 0) {
		    rs = SR_OK ;
	            i = (op->ti-1) ;
	        }
	        if (rpp) {
		    void	*vp = (void *) op->va[i] ;
	            *rpp = (rs >= 0) ? vp : nullptr ;
	        }
	    } /* end if (was open) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (raqhand_acclast) */

int raqhand_get(raqhand *op,int i,void **rpp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = raqhand_valid(op,i) ;
	        if (rpp) {
		    void	*vp = (void *) op->va[i] ;
	            *rpp = (rs >= 0) ? vp : nullptr ;
	        }
	    } /* end if (was open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (raqhand_get) */

int raqhand_rem(raqhand *op,void **rpp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = SR_NOTFOUND ;
	        if (op->c > 0) {
		    rs = SR_OK ;
	            i = op->hi ;
	            op->hi = ((op->hi+1) % op->n) ;
	            op->c -= 1 ;
	        }
	        if (rpp) {
		    void	*vp = (void *) op->va[i] ;
	            *rpp = (rs >= 0) ? vp : nullptr ;
	        } /* end if (non-null) */
	    } /* end if (was open) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (raqhand_rem) */

int raqhand_ent(raqhand *op,cvoid *cvp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && cvp) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = SR_NOTFOUND ;
	        if (op->c > 0) {
	            bool	f = false ;
		    rs = SR_OK ;
	            for (int j = 0 ; j < op->c ; j += 1) {
		        i = op->hi ;
	                if (op->va[i]) {
	                    f = (op->va[i] == cvp) ;
		            if (f) break ;
	                }
		        i = ((i+1) % op->n) ;
	            } /* end for */
	            if (! f) rs = SR_NOTFOUND ;
	        }
	    } /* end if (was open) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (raqhand_ent) */

int raqhand_del(raqhand *op,int i) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
	        if ((rs = raqhand_valid(op,i)) >= 0) {
	            op->va[i] = nullptr ;
	        } /* end if (valid) */
	    } /* end if (was open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (raqhand_del) */

int raqhand_delall(raqhand *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = SR_OK ;
	        op->hi = 0 ;
	        op->ti = 0 ;
	        op->c = 0 ;
	    } /* end if (was open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (raqhand_delall) */

int raqhand_count(raqhand *op) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = SR_OK ;
	        c = op->c ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (raqhand_count) */


/* private subroutines */

static int raqhand_setopts(raqhand *op,int vo) noex {
	op->f = {} ;
	(void) vo ;
	return SR_OK ;
}
/* end subroutine (raqhand_setopts) */

static int raqhand_valid(raqhand *op,int i) noex {
	int		rs = SR_NOTFOUND ;
	if (op->c > 0) {
	    rs = SR_OK ;
	    if (op->hi != op->ti) {
		if ((i < op->hi) || (i >= op->ti)) rs = SR_NOTFOUND ;
	    }
	}
	return rs ;
}
/* end subroutine (raqhand_valid) */

#ifdef	COMMENT
static int raqhand_extend(raqhand *op) noex {
	int		rs = SR_OK ;
	if ((op->i + 1) > op->n) {
	    int		nn, size ;
	    void	*np ;
	    if (op->va == nullptr) {
	        nn = RAQHAND_DEFENTS ;
	        size = (nn + 1) * sizeof(void **) ;
	        rs = uc_libmalloc(size,&np) ;
	    } else {
	        nn = (op->n + 1) * 2 ;
	        size = (nn + 1) * sizeof(void **) ;
	        rs = uc_librealloc(op->va,size,&np) ;
	        op->va = nullptr ;
	    }
	    if (rs >= 0) {
	        op->va = (cvoid **) np ;
	        op->n = nn ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (raqhand_extend) */
#endif /* COMMENT */



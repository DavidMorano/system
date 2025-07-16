/* density SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* this is a density gathering object */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-02-16, David A­D­ Morano
        This was written for some statistics gathering for some software
        evaluation.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	density

	Description:
	This object facilitates maintaining the probability density
	for some random variable.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"density.h"
#include	"densitystat.h"

import libutil ;

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int density_ctor(density *op,Args ... args) noex {
    	DENSITY		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (density_ctor) */

static int density_dtor(density *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (density_dtor) */

template<typename ... Args>
static inline int density_magic(density *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == DENSITY_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (density_magic) */

/* local variables */


/* exported variables */


/* exported subroutines */

int density_start(density *op,int len) noex {
	int		rs ;
	if ((rs = density_ctor(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (len > 0) {
	        cint	sz = (len + 1) * szof(ulong) ;
	        if (void *p ; (rs = uc_malloc(sz,&p)) >= 0) {
	            op->a = ulongp(p) ;
	            memclear(op->a,sz) ;
	            op->len = len ;
	            op->magic = DENSITY_MAGIC ;
	        } /* end if (m-a) */
	    } /* end if (valid) */
	    if (rs < 0) {
		density_dtor(op) ;
	    }
	} /* end if (density_ctor) */
	return rs ;
}
/* end subroutine (density_start) */

int density_finish(density *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = density_magic(op)) >= 0) {
	    if (op->a) {
	        rs1 = uc_free(op->a) ;
	        if (rs >= 0) rs = rs1 ;
	        op->a = nullptr ;
	    }
	    {
	        rs1 = density_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (density_finish) */

int density_update(density *op,int ai) noex {
	int		rs ;
	if ((rs = density_magic(op)) >= 0) {
	    if (ai > op->nmax) {
	        op->nmax = ai ;
	    }
	    if (ai > (op->len - 1)) {
	        rs = SR_RANGE ;
	        ai = (op->len - 1) ;
	        op->ovf += 1 ;
	    } /* end if */
	    op->c += 1 ;
	    op->a[ai] += 1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (density_update) */

int density_slot(density *op,int ai,ulong *rp) noex {
    	int		rs ;
	if ((rs = density_magic(op,rp)) >= 0) {
	    rs = SR_INVALID ;
	    if (ai < op->len) {
		rs = SR_OK ;
	        *rp = op->a[ai] ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (density_slot) */

int density_getstats(density *op,density_st *sp) noex {
	int		rs ;
	if ((rs = density_magic(op,sp)) >= 0) {
	    cint	len = int(op->len) ;
	    sp->mean = 0.0 ;
	    sp->var = 0.0 ;
	    sp->count = op->c ;
	    sp->nmax = op->nmax ;
	    sp->ovf = op->ovf ;
	    sp->len = op->len ;
	    if ((rs = densitystatl(op->a,len,&sp->mean,&sp->var)) >= 0) {
	        ulong	sum = 0 ;
	        for (int i = 0 ; i < op->len ; i += 1) {
	            sum += op->a[i] ;
	        }
	        if (sum != op->c) rs = SR_BADFMT ;
	    } /* end if */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (density_getstats) */



/* bits */
/* lang=C++20 */

/* perform some bit-array type operations */
/* version %I% last-modified %G% */


/* revistion history:

	= 1998-05-27, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module does some bit-array type [of] stuff.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<limits.h>		/* <- for |CHAR_BIT| */
#include	<cstdlib>
#include	<cstring>
#include	<bit>
#include	<usystem.h>
#include	<usupport.h>
#include	<stdintx.h>
#include	<baops.h>
#include	<intceil.h>
#include	<localmisc.h>

#include	"bits.h"


/* local defines */

#define	BITS_BPW	(CHAR_BIT * sizeof(BITS_DIGIT))
#define	BITS_NMINWORDS	1


/* local namespaces */

using std::popcount ;
using std::countr_zero ;


/* local typedefs */

typedef BITS_DIGIT	digit ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	bits_alloc(bits *,int) noex ;


/* local variables */


/* exported subroutines */

int bits_start(bits *op,int n) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_INVALID ;
	    if (n >= 0) {
		rs = SR_OK ;
	        op->a = nullptr ;
	        op->nwords = 0 ;
	        op->nbits = 0 ;
	        op->n = 0 ;
	        if (n > 0) {
	            rs = bits_alloc(op,n) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bits_start) */

int bits_finish(bits *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->a) {
	        rs1 = uc_free(op->a) ;
	        if (rs >= 0) rs = rs1 ;
	        op->a = nullptr ;
	    }
	    op->nwords = 0 ;
	    op->nbits = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bits_finish) */

int bits_set(bits *op,int i) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_INVALID ;
	    if (i >= 0) {
		rs = SR_OK ;
	        if (i >= op->nbits) {
	            rs = bits_alloc(op,i+1) ;
	        }
	        if (rs >= 0) {
	            if ((i+1) > op->n) op->n = (i+1) ;
	            rs = batst(op->a,i) ;
	            baset(op->a,i) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bits_set) */

int bits_clear(bits *op,int i) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_INVALID ;
	    if (i >= 0) {
		rs = SR_OK ;
	        if (i >= op->nbits) {
	            rs = bits_alloc(op,i+1) ;
	        }
	        if (rs >= 0) {
	            if (i >= 0) {
	                if ((i+1) > op->n) op->n = (i+1) ;
	                rs = batst(op->a,i) ;
	                baclr(op->a,i) ;
	            } else if (op->a) {
		        memclear(op->a,op->nwords) ;
	            }
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bits_clear) */

int bits_test(bits *op,int i) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_INVALID ;
	    if (i >= 0) {
		rs = SR_OK ;
	        if (i < op->nbits) {
	            rs = batst(op->a,i) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bits_test) */

int bits_anyset(bits *op) noex {
	int		rs = SR_FAULT ; 
	int		f = false ;
	if (op) {
	    rs = SR_OK ;
	    for (int w = 0 ; w < op->nwords ; w += 1) {
	       f = (op->a[w] != 0) ;
	       if (f) break ;
	    } /* end for */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (bits_anyset) */

int bits_ffbs(bits *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    int		w = 0 ;
	    rs = SR_NOTFOUND ;
	    while ((w < op->nwords) && (op->a[w] == 0)) {
		w += 1 ;
	    } /* end while */
	    if (w < op->nwords) {
	        constexpr int	bpw = BITS_BPW ;
	        rs = countr_zero(op->a[w]) + (w * bpw) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bits_ffbs) */

int bits_extent(bits *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->n ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bits_extent) */

int bits_count(bits *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    for (int w = 0 ; w < op->nwords ; w += 1) {
		rs += popcount(op->a[w]) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bits_count) */


/* private subroutines */

static int bits_alloc(bits *op,int nn) noex {
	int		rs = SR_OK ;
	int		nnwords = ((nn + (BITS_BPW-1))/BITS_BPW) ;
	if (nnwords > op->nwords) {
	    cint	nminwords = (op->nwords+BITS_NMINWORDS) ;
	    int		size ;
	    caddr_t	na{} ;
	    if (nnwords < nminwords) nnwords = nminwords ;
	    size = nnwords * sizeof(digit) ;
	    if (op->a == nullptr) {
	        rs = uc_malloc(size,&na) ;
	        if (rs >= 0) memclear(na,size) ;
	    } else {
		cint	osz = (op->nwords*sizeof(digit)) ;
		int	newsize ;
		newsize = (size - osz) ;
	        rs = uc_realloc(op->a,size,&na) ;
	        if (rs >= 0) memclear((na+osz),newsize) ;
	    }
	    if (rs >= 0) {
	        op->a = (digit *) na ;
	        op->nwords = nnwords ;
	        op->nbits = (nnwords * BITS_BPW) ;
	    }
	} /* end if (allocation or reallocation needed) */
	return rs ;
}
/* end subroutine (bits_alloc) */

int bits_co::operator () (int a) noex {
	int	rs = SR_BUGCHECK ;
	switch (w) {
	case bitsmem_start:
	    rs = bits_start(op,a) ;
	    break ;
	case bitsmem_finish:
	    rs = bits_finish(op) ;
	    break ;
	case bitsmem_set:
	    rs = bits_set(op,a) ;
	    break ;
	case bitsmem_clear:
	    rs = bits_clear(op,a) ;
	    break ;
	case bitsmem_test:
	    rs = bits_test(op,a) ;
	    break ;
	case bitsmem_anyset:
	    rs = bits_anyset(op) ;
	    break ;
	case bitsmem_ffbs:
	    rs = bits_ffbs(op) ;
	    break ;
	case bitsmem_extent:
	    rs = bits_extent(op) ;
	    break ;
	case bitsmem_count:
	    rs = bits_count(op) ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end method (bits_co::operator) */

int bits_co::operator [] (int a) noex {
	return (*this)(a) ;
}



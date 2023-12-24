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
#include	<climits>		/* <- for |CHAR_BIT| */
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

#define	BITS_BPW	(CHAR_BIT * sizeof(BITS_TYPEDIGIT))
#define	BITS_NMINWORDS	1


/* local namespaces */

using std::popcount ;
using std::countr_zero ;


/* local typedefs */

typedef BITS_TYPEDIGIT	digit ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static inline void	bits_naclear(bits *) noex ;
static int		bits_alloc(bits *,int) noex ;
static int		bits_allocx(bits *,int) noex ;

static int		ffbsarr(digit *,int) noex ;


/* local variables */

constexpr int		nawords = BITS_SHORTDIGS ;
constexpr int		nabits = (BITS_SHORTDIGS * BITS_BPW) ;


/* exported variables */


/* exported subroutines */

int bits_start(bits *op,int n) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_INVALID ;
	    if (n >= 0) {
		rs = SR_OK ;
	        op->a = nullptr ;
	        op->nwords = nawords ;
	        op->nbits = nabits ;
	        op->n = 0 ;
		bits_naclear(op) ;
	        if (n > op->nbits) {
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
		    if (i >= nabits) {
			cint	ii = (i - nabits) ;
	                rs = batst(op->a,ii) ;
	                baset(op->a,ii) ;
		    } else {
	                rs = batst(op->na,i) ;
	                baset(op->na,i) ;
		    }
	        } /* end if (ok) */
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
	            if (i >= 0) { /* clear one bit */
	                if ((i+1) > op->n) op->n = (i+1) ;
			if (i >= nabits) {
			    cint	ii = (i - nabits) ;
	                    rs = batst(op->a,ii) ;
	                    baclr(op->a,ii) ;
			} else {
	                    rs = batst(op->na,i) ;
	                    baclr(op->na,i) ;
			}
	            } else {
		        bits_naclear(op) ;
			if (op->a) { /* clear all bits */
		            memclear(op->a,(op->nwords - nawords)) ;
			}
	            }
	        } /* end if (ok) */
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
		    if (i >= nabits) {
			cint	ii = (i - nabits) ;
	                rs = batst(op->a,ii) ;
		    } else {
	                rs = batst(op->na,i) ;
		    }
	        } /* end if (within range) */
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
	    if (!f) {
	        for (int w = 0 ; w > nawords ; w += 1) {
	            f = (op->a[w] != 0) ;
	            if (f) break ;
	        }
	    }
	    if (!f) {
	        for (int w = 0 ; w < (op->nwords - nawords) ; w += 1) {
	            f = (op->a[w] != 0) ;
	            if (f) break ;
	        } /* end for */
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (bits_anyset) */

int bits_ffbs(bits *op) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = ffbsarr(op->na,nawords)) == rsn) {
		if (op->a) {
		    cint	nw = (op->nwords - nawords) ;
		    if ((rs = ffbsarr(op->a,nw)) >= 0) {
			rs += nabits ;
		    }
		} /* end if (allocation) */
	   } /* end if */
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
	    for (int w = 0 ; w < nawords ; w += 1) {
		rs += popcount(op->na[w]) ;
	    }
	    for (int w = 0 ; w < (op->nwords - nawords) ; w += 1) {
		rs += popcount(op->a[w]) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bits_count) */


/* private subroutines */

static void bits_naclear(bits *op) noex {
	cint	nabytes = (nawords * sizeof(digit)) ;
	memclear(op->na,nabytes) ;
}
/* end subroutine (bits_naclear) */

static int bits_alloc(bits *op,int nn) noex {
	int		rs = SR_OK ;
	if (nn >= nabits) {
	    rs = bits_allocx(op,nn) ;
	}
	return rs ;
}
/* end subroutine (bits::alloc) */

static int bits_allocx(bits *op,int nn) noex {
	cint		nminwords = (op->nwords+BITS_NMINWORDS) ;
	int		rs = SR_OK ;
	int		nnwords = ((nn + (BITS_BPW-1))/BITS_BPW) ;
	if (nnwords > op->nwords) {
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
	    } /* end if (ok) */
	} /* end if (allocation or reallocation needed) */
	return rs ;
}
/* end subroutine (bits::allocx) */

void bits::dtor() noex {
	int	rs = finish ;
	if (rs < 0) {
	    ulogerror("bits",rs,"fini-fini") ;
	}
}
/* end subroutine (bits::dtor) */

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
/* end method (bits_co::operator) */

static int ffbsarr(digit *a,int an) noex {
	int		rs = SR_NOTFOUND ;
	int		w = 0 ;
	while ((w < an) && (a[w] == 0)) {
	    w += 1 ;
	} /* end while */
	if (w < an) {
	    rs = countr_zero(a[w]) + (w * BITS_BPW) ;
	}
	return rs ;
}
/* end subroutine (ffbsarr) */



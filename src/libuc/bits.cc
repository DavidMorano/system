/* bits SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* perform some bit-array type operations */
/* version %I% last-modified %G% */


/* revistion history:

	= 1998-02-15, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	bits

	Description:
	This object-module does some bit-array type [of] stuff.

	Implemention-node:
	I switch between "short-bit-optimization" and regular
	allocated bit storage as the need develops.  The old bits
	in non-allocated storage are not carried over to any allocation
	but rather are continued to be used in place.  Was this the
	proper design decision?  My initial guess was NO, it was
	not.  But I went ahead and coded it up anyway.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* <- for |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<bit>			/* <- |countr_zero(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<stdintx.h>
#include	<baops.h>
#include	<intceil.h>
#include	<localmisc.h>

#include	"bits.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	BITS_BPW	(CHAR_BIT * szof(BITS_TYPEDIGIT))
#define	BITS_MINWORDS	4


/* imported namespaces */

using std::popcount ;			/* subroutine (template) */
using std::countr_zero ;		/* subroutine (template) */
using libuc::libmem ;			/* variable */


/* local typedefs */

typedef BITS_TYPEDIGIT	digit ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct alloc {
	typedef digit	*digitp ;
	digit		*a ;
	int		asz ;
	alloc(digit *p,int sz) noex : a(p), asz(sz) { } ;
	int resize(int nsz) noex ;
    } ; /* end struct (alloc) */
}


/* forward references */

static inline void	bits_naclear(bits *) noex ;
static int		bits_alloc(bits *,int) noex ;

static int		nsizecalc(int,int) noex ;
static int		ffbsarr(digit *,int) noex ;


/* local variables */

constexpr int		minwords = BITS_MINWORDS ;
constexpr int		nawords = BITS_SHORTDIGS ;
constexpr int		nabits = (BITS_SHORTDIGS * BITS_BPW) ;
constexpr int		dsz = int(szof(digit)) ;


/* exported variables */


/* exported subroutines */

int bits_start(bits *op,int n) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_INVALID ;
	    if (n >= 0) ylikely {
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
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->a) {
	        rs1 = libmem.free(op->a) ;
	        if (rs >= 0) rs = rs1 ;
	        op->a = nullptr ;
	    }
	    op->nwords = 0 ;
	    op->nbits = 0 ;
	    op->n = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bits_finish) */

int bits_set(bits *op,int i) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_INVALID ;
	    if (i >= 0) ylikely {
		rs = SR_OK ;
	        if (i >= op->nbits) {
	            rs = bits_alloc(op,i+1) ;
	        }
	        if (rs >= 0) ylikely {
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
	if (op) ylikely {
	    rs = SR_OK ;
	    if (i >= 0) ylikely { /* <- clear one bit */
	        if (i >= op->nbits) {
	            rs = bits_alloc(op,i+1) ;
	        }
	        if (rs >= 0) ylikely {
	            if ((i + 1) > op->n) op->n = (i + 1) ;
		    if (i >= nabits) {
			cint	ii = (i - nabits) ;
	                rs = batst(op->a,ii) ;
	                baclr(op->a,ii) ;
		    } else {
	                rs = batst(op->na,i) ;
	                baclr(op->na,i) ;
		    }
	        } /* end if (ok) */
	    } else { /* <- clear all bits */
		bits_naclear(op) ;
	        if (op->a) {
		    cint nbytes = ((op->nwords - nawords) * szof(digit)) ;
		    memclear(op->a,nbytes) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bits_clear) */

int bits_test(bits *op,int i) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_INVALID ;
	    if (i >= 0) ylikely {
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
	if (op) ylikely {
	    rs = SR_OK ;
	    if (!f) {
	        for (int w = 0 ; w > nawords ; w += 1) {
	            f = (op->a[w] != 0) ;
	            if (f) break ;
	        } /* end for */
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
	if (op) ylikely {
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
	if (op) ylikely {
	    rs = op->n ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bits_extent) */

int bits_count(bits *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
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
	cint	nabytes = (nawords * szof(digit)) ;
	memclear(op->na,nabytes) ;
}
/* end subroutine (bits_naclear) */

static int bits_alloc(bits *op,int nn) noex {
	cint		bpw = BITS_BPW ;
	cint		asz = ((op->nwords - nawords) * dsz) ;
	int		rs = SR_OK ;
	if (nn >= op->nbits) {
	    alloc	ao(op->a,asz) ;
	    cint	nsz = nsizecalc(op->nbits,nn) ;
	    if ((rs = ao.resize(nsz)) > 0) {
	        op->a = ao.a ;
	        op->nwords = ((rs / dsz) + nawords) ;
		op->nbits = (((rs / dsz) + nawords) * bpw) ;
	    }
	} /* end if (needed) */
	return rs ;
}
/* end subroutine (bits_alloc) */

void bits::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("bits",rs,"dtor-finish") ;
	}
}
/* end subroutine (bits::dtor) */

int bits_co::operator () (int a) noex {
	int	rs = SR_BUGCHECK ;
	if (op) ylikely {
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
	} /* end if (non-null) */
	return rs ;
}
/* end method (bits_co::operator) */

int bits_co::operator [] (int a) noex {
	return (*this)(a) ;
}
/* end method (bits_co::operator) */

int alloc::resize(int nsz) noex {
	int		rs = SR_OK ;
	int		rsize = 0 ;
	if (nsz > asz) {
	    caddr_t	na{} ;
	    if (a == nullptr) {
	        if ((rs = libmem.mall(nsz,&na)) >= 0) {
	            memclear(na,nsz) ;
		}
	    } else {
	        if ((rs = libmem.rall(a,nsz,&na)) >= 0) {
	            memclear((na + asz),(nsz - asz)) ;
		}
	    }
	    if (rs >= 0) {
	        a = digitp(na) ;
		asz = nsz ;
		rsize = nsz ;
	    } /* end if (ok) */
	} /* end if (allocation or reallocation needed) */
	return (rs >= 0) ? rsize : rs ;
}
/* end method (alloc::resize) */

static int nsizecalc(int obits,int nbits) noex {
	int		nw = iceil((nbits - obits),BITS_BPW) ;
	int		nsz = 0 ;
	if (nw > nawords) {
	    nw -= nawords ;
	    if (nw < minwords) nw = minwords ;
	    nsz = (nw * dsz) ;
	}
	return nsz ;
}
/* end subroutine (nsizecalc) */

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



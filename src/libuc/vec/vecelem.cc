/* vecelem SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* vector element-list operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	vecelem

	Description:
	These routines are used when the caller wants to store a
	COPY of the passed element data into a vector.  These
	routines will copy and store the copied data in the list.
	The advantage is that the caller does not have to keep the
	orginal data around in order for the list data to be accessed
	later.  Element data (unlike string data) can contain nullptr
	characters-bytes.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<localmisc.h>

#include	"vecelem.h"

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| + |memcopy(3u)| */

/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using libuc::libmem ;			/* variable */


/* local typedefs */

typedef vecelem_vcmp	c_f ;

extern "C" {
    typedef int (*qsort_f)(cvoid *,cvoid *) noex ;
    typedef int (*rawc_f)(void **,void **) noex ;
    typedef int (*vg_f)(vecelem *,int,void **) noex ;
}


/* forward references */

static int	vecelem_ctor(vecelem *) noex ;
static int	vecelem_dtor(vecelem *) noex ;
static int	vecelem_extend(vecelem *) noex ;
static int	vecelem_setopts(vecelem *,int) noex ;

template<typename ... Args>
static inline int vecelem_magic(vecelem *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == VECELEM_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (vecelem_magic) */

consteval int mkoptmask() noex {
	int		m = 0 ;
	m |= VECELEM_OREUSE ;
	m |= VECELEM_OCOMPACT ;
	m |= VECELEM_OSWAP ;
	m |= VECELEM_OSTATIONARY ;
	m |= VECELEM_OCONSERVE ;
	m |= VECELEM_OSORTED ;
	m |= VECELEM_OORDERED ;
	return m ;
} /* end subroutine (mkoptmask) */


/* local variables */

constexpr int		optmask = mkoptmask() ;


/* exported variables */


/* exported subroutines */

int vecelem_start(vecelem *op,int esz,int n,int opts) noex {
	int		rs ;
	if ((rs = vecelem_ctor(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (esz > 0) {
	        if (n < 0) n = VECELEM_DEFENTS ;
	        op->esz = esz ;
	        if ((rs = vecelem_setopts(op,opts)) >= 0) {
	            if (n > 0) {
	                cint	sz = (n + 1) * op->esz ;
	                if (void *p ; (rs = libmem.mall(sz,&p)) >= 0) {
	                    op->va = voidpp(p) ;
	    	            op->n = n ;
		            op->magic = VECELEM_MAGIC ;
	                } /* end if (memory-allocation) */
	            }
	        } /* end if */
	    } /* end if (valid) */
	    if (rs < 0) {
		vecelem_dtor(op) ;
	    }
	} /* end if (vecelem_ctor) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (vecelem_start) */

int vecelem_finish(vecelem *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = vecelem_magic(op)) >= 0) {
	    if (op->va) {
		rs1 = libmem.free(op->va) ;
		if (rs >= 0) rs = rs1 ;
		op->va = nullptr ;
	    }
	    {
		rs1 = vecelem_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->c = 0 ;
	    op->i = 0 ;
	    op->n = 0 ;
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecelem_finish) */

int vecelem_add(vecelem *op,cvoid *ep) noex {
	int		rs ;
	int		i = 0 ;
	if ((rs = vecelem_magic(op,ep)) >= 0) {
	        if ((op->i + 1) > op->n) {
	            rs = vecelem_extend(op) ;
	        }
	        if (rs >= 0) {
	            voidpp vep = op->va ;
	            i = op->i ;
	            vep += (i * op->esz) ;
	            memcopy(vep,ep,op->esz) ;
	            op->i = (i + 1) ;
	            op->c += 1 ;	/* increment list count */
	            op->fl.issorted = false ;
	        } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecelem_add) */

int vecelem_adduniq(vecelem *op,cvoid *ep) noex {
	int		rs ;
	if ((rs = vecelem_magic(op,ep)) >= 0) {
	    cint	esz = op->esz ;
	    int	i{} ;
	    rs = INT_MAX ;
	    for (i = 0 ; i < op->i ; i += 1) {
	        voidpp vep = op->va ;
	        vep += (i * esz) ;
	        if (memcmp(vep,ep,esz) == 0) break ;
	    } /* end for */
	    if (i >= op->i) {
	        rs = vecelem_add(op,ep) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecelem_adduniq) */

int vecelem_get(vecelem *op,int i,void *vrp) noex {
	int		rs ;
	if ((rs = vecelem_magic(op,vrp)) >= 0) {
	    rs = SR_NOTFOUND ;
	    if ((i >= 0) && (i < op->i)) {
	        voidpp	cvpp = voidpp(vrp) ;
	        voidpp	vep = op->va ;
		rs = SR_OK ;
	        vep += (i * op->esz) ;
	        *cvpp = vep ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecelem_get) */

int vecelem_getval(vecelem *op,int i,void *vrp) noex {
	int		rs ;
	if ((rs = vecelem_magic(op,vrp)) >= 0) {
	    rs = SR_NOTFOUND ;
	    if ((i >= 0) && (i < op->i)) {
	        voidpp	cvp = voidpp(vrp) ;
	        voidpp	vep = op->va ;
	        rs = SR_OK ;
	        vep += (i * op->esz) ;
	        memcopy(cvp,vep,op->esz) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecelem_getval) */

int vecelem_count(vecelem *op) noex {
	int		rs ;
	if ((rs = vecelem_magic(op)) >= 0) {
		rs = op->c ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecelem_count) */

int vecelem_sort(vecelem *op,vecelem_vcmp vcf) noex {
	int		rs ;
	if ((rs = vecelem_magic(op,vcf)) >= 0) {
	        rs = op->c ; 
	        if (! op->fl.issorted) {
	            op->fl.issorted = true ;
	            if (op->c > 1) {
			qsort_f	scf = qsort_f(vcf) ;
		        cint	esz = op->esz ;
	                qsort(op->va,op->i,esz,scf) ;
	            }
		}
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecelem_sort) */

int vecelem_setsorted(vecelem *op) noex {
	int		rs ;
	if ((rs = vecelem_magic(op)) >= 0) {
	        rs = op->c ; 
		op->fl.issorted = true ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecelem_setsorted) */

int vecelem_getvec(vecelem *op,void **rpp) noex {
	int		rs ;
	if ((rs = vecelem_magic(op,rpp)) >= 0) {
		rs = op->i ;
	        *rpp = op->va ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecelem_getvec) */

int vecelem_audit(vecelem *op) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = vecelem_magic(op)) >= 0) {
		if (op->va) {
	            {
	                uintptr_t 	v = uintptr_t(op->va) ;
	                if ((v & 3) != 0) rs = SR_BADFMT ;
	            }
	            if (rs >= 0) {
	                cint	esz = op->esz ;
	                if (void *ep ; (rs = libmem.mall(esz,&ep)) >= 0) {
		            int		i ; /* used-afterwards */
	                    for (i = 0 ; i < op->i ; i += 1) {
		                voidpp	cap = op->va ;
		                cap += (i * esz) ;
	                        memcopy(ep,cap,esz) ;
	                    } /* end for */
	                    c = op->c ;
	                    rs = (i == c) ? SR_OK : SR_BADFMT ;
			    {
	                        rs1 = libmem.free(ep) ;
	                        if (rs >= 0) rs = rs1 ;
			    }
	                } /* end if (m-a-f) */
	            } /* end if (ok) */
		} /* end if (non-nullptr va) */
	        if (rs >= 0) {
	            if ((op->i > op->n) || (op->c > op->i)) rs = SR_BADFMT ;
	        }
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecelem_audit) */


/* private subroutines */

static int vecelem_ctor(vecelem *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->va = nullptr ;
	    op->c = 0 ;
	    op->i = 0 ;
	    op->n = 0 ;
	    op->fi = 0 ;
	    op->esz = 0 ;
	    op->magic = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecelem_ctor) */

static int vecelem_dtor(vecelem *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (vecelem_dtor) */

static int vecelem_setopts(vecelem *op,int vo) noex {
	int		rs = SR_INVALID ;
	if ((vo & (~ optmask)) == 0) {
	    rs = SR_OK ;
	    op->fl = {} ;
	    if (vo & VECELEM_OREUSE)		op->fl.oreuse = 1 ;
	    if (vo & VECELEM_OSWAP)		op->fl.oswap = 1 ;
	    if (vo & VECELEM_OSTATIONARY)	op->fl.ostationary = 1 ;
	    if (vo & VECELEM_OCOMPACT)		op->fl.ocompact = 1 ;
	    if (vo & VECELEM_OSORTED)		op->fl.osorted = 1 ;
	    if (vo & VECELEM_OORDERED)		op->fl.oordered = 1 ;
	    if (vo & VECELEM_OCONSERVE)		op->fl.oconserve = 1 ;
	}
	return rs ;
}
/* end subroutine (vecelem_setopts) */

static int vecelem_extend(vecelem *op) noex {
	int		rs = SR_OK ;
	if ((op->i + 1) > op->n) {
	    int		nn ;
	    int		sz ;
	    void	*va ;
	    if (op->va == nullptr) {
	        nn = VECELEM_DEFENTS ;
	        sz = (nn + 1) * op->esz ;
	        rs = libmem.mall(sz,&va) ;
	    } else {
	        nn = (op->n + 1) * 2 ;
	        sz = (nn + 1) * op->esz ;
	        rs = libmem.rall(op->va,sz,&va) ;
	    } /* end if */
	    if (rs >= 0) {
	        op->va = voidpp(va) ;
	        op->n = nn ;
		op->va[op->i] = nullptr ;
	    }
	} /* end if (extension required) */
	return rs ;
}
/* end subroutine (vecelem_extend) */



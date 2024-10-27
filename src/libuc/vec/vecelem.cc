/* vecelem SUPPORT */
/* encoding=ISO8859-1 */
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
#include	<cstring>		/* |memcpy(2c)| */
#include	<algorithm>
#include	<usystem.h>
#include	<localmisc.h>

#include	"vecelem.h"


/* local defines */


/* imported namespaces */

using std::min ;


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
}


/* local subroutines */

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
}
/* end subroutine (mkoptmask) */


/* local variables */

constexpr int		optmask = mkoptmask() ;


/* exported variables */


/* exported subroutines */

int vecelem_start(vecelem *op,int esize,int n,int opts) noex {
	int		rs ;
	if ((rs = vecelem_ctor(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (esize > 0) {
	        if (n < 0) n = VECELEM_DEFENTS ;
	        op->esize = esize ;
	        if ((rs = vecelem_setopts(op,opts)) >= 0) {
	            if (n > 0) {
	                cint	sz = (n + 1) * op->esize ;
	                char	*p ;
	                if ((rs = uc_libmalloc(sz,&p)) >= 0) {
	                    op->va = (void **) p ;
	    	            op->n = n ;
		            op->magic = VECELEM_MAGIC ;
	                }
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
		rs1 = uc_libfree(op->va) ;
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
	            caddr_t	vep = caddr_t(op->va) ;
	            i = op->i ;
	            vep += (i * op->esize) ;
	            memcpy(vep,ep,op->esize) ;
	            op->i = (i+1) ;
	            op->c += 1 ;	/* increment list count */
	            op->f.issorted = false ;
	        } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecelem_add) */

int vecelem_adduniq(vecelem *op,cvoid *ep) noex {
	int		rs ;
	if ((rs = vecelem_magic(op,ep)) >= 0) {
	    cint	esize = op->esize ;
	    int	i{} ;
	    rs = INT_MAX ;
	    for (i = 0 ; i < op->i ; i += 1) {
	        caddr_t	vep = caddr_t(op->va) ;
	        vep += (i * esize) ;
	        if (memcmp(vep,ep,esize) == 0) break ;
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
	        caddr_t		*cvpp = (caddr_t *) vrp ;
	        caddr_t		vep = caddr_t(op->va) ;
		rs = SR_OK ;
	        vep += (i * op->esize) ;
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
	        caddr_t	cvp = caddr_t(vrp) ;
	        caddr_t	vep = caddr_t(op->va) ;
	        rs = SR_OK ;
	        vep += (i * op->esize) ;
	        memcpy(cvp,vep,op->esize) ;
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
	        if (! op->f.issorted) {
	            op->f.issorted = true ;
	            if (op->c > 1) {
			qsort_f	scf = qsort_f(vcf) ;
		        cint	esize = op->esize ;
	                qsort(op->va,op->i,esize,scf) ;
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
		op->f.issorted = true ;
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
	                uintptr_t 	v = (long) op->va ;
	                if ((v & 3) != 0) rs = SR_BADFMT ;
	            }
	            if (rs >= 0) {
		        void	*ep = nullptr ;
	                cint	esize = op->esize ;
	                if ((rs = uc_libmalloc(esize,&ep)) >= 0) {
		            int		i ; /* used-afterwards */
	                    for (i = 0 ; i < op->i ; i += 1) {
		                caddr_t		cap = caddr_t(op->va) ;
		                cap += (i * esize) ;
	                        memcpy(ep,cap,esize) ;
	                    } /* end for */
	                    c = op->c ;
	                    rs = (i == c) ? SR_OK : SR_BADFMT ;
	                    rs1 = uc_libfree(ep) ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (memory-allocation) */
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
	    op->esize = 0 ;
	    op->magic = 0 ;
	}
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
	if ((vo & (~optmask)) == 0) {
	    rs = SR_OK ;
	    op->f = {} ;
	    if (vo & VECELEM_OREUSE) op->f.oreuse = 1 ;
	    if (vo & VECELEM_OSWAP) op->f.oswap = 1 ;
	    if (vo & VECELEM_OSTATIONARY) op->f.ostationary = 1 ;
	    if (vo & VECELEM_OCOMPACT) op->f.ocompact = 1 ;
	    if (vo & VECELEM_OSORTED) op->f.osorted = 1 ;
	    if (vo & VECELEM_OORDERED) op->f.oordered = 1 ;
	    if (vo & VECELEM_OCONSERVE) op->f.oconserve = 1 ;
	}
	return rs ;
}
/* end subroutine (vecelem_setopts) */

static int vecelem_extend(vecelem *op) noex {
	int		rs = SR_OK ;
	if ((op->i + 1) > op->n) {
	    int		nn, size ;
	    void	*va ;
	    if (op->va == nullptr) {
	        nn = VECELEM_DEFENTS ;
	        size = (nn + 1) * op->esize ;
	        rs = uc_libmalloc(size,&va) ;
	    } else {
	        nn = (op->n + 1) * 2 ;
	        size = (nn + 1) * op->esize ;
	        rs = uc_librealloc(op->va,size,&va) ;
	    } /* end if */
	    if (rs >= 0) {
	        op->va = va ;
	        op->n = nn ;
		op->va[op->i] = nullptr ;
	    }
	} /* end if (extension required) */
	return rs ;
}
/* end subroutine (vecelem_extend) */



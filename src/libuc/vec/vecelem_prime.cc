/* vecelem_prime SUPPORT */
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
	later.  Element data (unlike string data) can contain NUL
	characters-bytes.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |qsort(3c)| */
#include	<cstring>		/* |memcmp(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<localmisc.h>
#include	<debprintf.h>

#include	"vecelem.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/debug.ccm"

import libutil ;			/* |lenstr(3u)| + |memcopy(3u)| */
import debug ;

/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */

typedef vecelem_vcmp	c_f ;

extern "C" {
    typedef int (*qsort_f)(cvoid *,cvoid *) noex ;
    typedef int (*rawc_f)(void **,void **) noex ;
    typedef int (*vg_f)(vecelem *,int,void **) noex ;
}


/* forward references */

local int	vecelem_ctor(vecelem *) noex ;
local int	vecelem_dtor(vecelem *) noex ;
local int	vecelem_setopts(vecelem *,int) noex ;
local int	vecelem_extend(vecelem *) noex ;
local int	vecelem_validx(vecelem *,int) noex ;

template<typename ... Args>
local inline int vecelem_magic(vecelem *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magic == VECELEM_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (vecelem_magic) */


/* local variables */


/* exported variables */

constexpr vecelemms	vecelemm ;


/* exported subroutines */

int vecelem_start(vecelem *op,int esz,int vn,int vo) noex {
	int		rs ;
	if ((rs = vecelem_ctor(op)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (esz > 0) ylikely {
	        if (vn <= 0) vn = VECELEM_DEFENTS ;
	        op->esz = esz ;
	        if ((rs = vecelem_setopts(op,vo)) >= 0) ylikely {
	            cint	sz = (vn + 1) * op->esz ;
	            if (void *p ; (rs = libmem.mall(sz,&p)) >= 0) {
	                op->va = p ;
	    	        op->n = vn ;
		    	op->magic = VECELEM_MAGIC ;
	            } /* end if (memory-allocation) */
	        } /* end if */
	    } /* end if (valid) */
	    if (rs < 0) {
		vecelem_dtor(op) ;
	    }
	} /* end if (vecelem_ctor) */
	return (rs >= 0) ? vn : rs ;
}
/* end subroutine (vecelem_start) */

int vecelem_finish(vecelem *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = vecelem_magic(op)) >= 0) ylikely {
	    if (op->va) ylikely {
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
	int		i = 0 ; /* extern-value */
	if ((rs = vecelem_magic(op,ep)) >= 0) ylikely {
	    if ((rs = vecelem_extend(op)) >= 0) ylikely {
	        caddr_t vp = caddr_t(op->va) ;
	        i = op->i ;
	        vp += (i * op->esz) ;
	        memcopy(vp,ep,op->esz) ;
	        op->i = (i + 1) ;
	        op->c += 1 ;	/* increment list count */
	        op->fl.issorted = false ;
	    } /* end if (vecelem_ext) */
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecelem_add) */

int vecelem_addlist(vecelem *op,cvoid *listp,int listl) noex {
    	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op && listp) {
	    rs = SR_INVALID ;
	    if (listl >= 0) {
		caddr_t	sp = caddr_t(listp) ;
	        cint	esz = op->esz ;
		rs = SR_OK ;
	        for (int i = 0 ; i < listl ; i += 1) {
	            rs = vecelem_add(op,sp) ;
		    sp += esz ;
	            c += 1 ;
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if (value) */
	} /* end if (non-null) */
    	return (rs >= 0) ? c : rs ;
} /* end subroutine (vecelem_addlist) */

int vecelem_adduniq(vecelem *op,cvoid *ep) noex {
	int		rs ;
	if ((rs = vecelem_magic(op,ep)) >= 0) ylikely {
	    caddr_t	vp = caddr_t(op->va) ;
	    csize	esize = size_t(op->esz) ;
	    int	i{} ; /* used-afterwards */
	    rs = INT_MAX ;
	    for (i = 0 ; i < op->i ; i += 1) {
	        if (memcmp(vp,ep,esize) == 0) break ;
	        vp += esize ;
	    } /* end for */
	    if (i >= op->i) {
	        rs = vecelem_add(op,ep) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecelem_adduniq) */

int vecelem_get(vecelem *op,int i,void *arp) noex {
	int		rs ;
	if ((rs = vecelem_magic(op,arp)) >= 0) ylikely {
	    rs = SR_NOTFOUND ;
	    if ((i >= 0) && (i < op->i)) {
	        voidpp	rpp = voidpp(arp) ;
	        caddr_t	vp = caddr_t(op->va) ;
		rs = SR_OK ;
	        vp += (i * op->esz) ;
	        *rpp = vp ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecelem_get) */

int vecelem_getval(vecelem *op,int i,void *vrp) noex {
	int		rs ;
	if ((rs = vecelem_magic(op,vrp)) >= 0) ylikely {
	    rs = SR_NOTFOUND ;
	    if ((i >= 0) && (i < op->i)) {
	        caddr_t	vp = caddr_t(op->va) ;
	        rs = SR_OK ;
	        vp += (i * op->esz) ;
	        memcopy(vrp,vp,op->esz) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecelem_getval) */

int vecelem_count(vecelem *op) noex {
	int		rs ;
	if ((rs = vecelem_magic(op)) >= 0) ylikely {
	    rs = op->c ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecelem_count) */

int vecelem_extent(vecelem *op) noex {
	int		rs ;
	if ((rs = vecelem_magic(op)) >= 0) ylikely {
	    rs = op->n ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecelem_extent) */

int vecelem_del(vecelem *op,int i) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->va) ylikely {
		if ((rs = vecelem_validx(op,i)) >= 0) ylikely {
	            caddr_t	vep = caddr_t(op->va) ;
	            bool	f_fi = false ;
		    if (op->fl.ostationary) {
	        	vep += (i * op->esz) ;
	    		if (i == (op->i - 1)) op->i -= 1 ;
	    		f_fi = true ;
		    } else if (op->fl.issorted || op->fl.oordered) {
	                if (op->fl.ocompact) {
			    op->i -= 1 ;
			    caddr_t	dent = (vep + ((i+0) * op->esz)) ;
			    caddr_t	sent = (vep + ((i+1) * op->esz)) ;
	                    for (int j = i ; j < op->i ; j += 1) {
	        	        memcopy(dent,sent,op->esz) ;
				sent += op->esz ;
				dent += op->esz ;
		            } /* end for */
			    memclear(dent,op->esz) ;
	                } else {
			    caddr_t	dent = (vep + ((i+0) * op->esz)) ;
			    memclear(dent,op->esz) ;
	                    if (i == (op->i - 1)) op->i -= 1 ;
	                    f_fi = true ;
	                } /* end if */
	            } else {
			bool	f = (op->fl.oswap || op->fl.ocompact)  ;
			f = f && (i < (op->i - 1)) ;
			if (f) {
			    caddr_t	dent = (vep + ((i+0) * op->esz)) ;
			    caddr_t	sent = (vep + ((op->i-1) * op->esz)) ;
	        	    memcopy(dent,sent,op->esz) ;
			    op->i -= 1 ;
			    memclear(sent,op->esz) ;
	                    op->fl.issorted = false ;
	                } else {
			    caddr_t	dent = (vep + ((i+0) * op->esz)) ;
			    memclear(dent,op->esz) ;
	                    if (i == (op->i - 1)) op->i -= 1 ;
	                    f_fi = true ;
	                } /* end if */
	            } /* end if */
		    op->c -= 1 ;
	            if (f_fi && (i < op->fi)) {
			op->fi = i ;
		    }
		} /* end if (valid index) */
	    } /* end if (popularion) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecelem_del) */

int vecelem_delall(vecelem *op) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = vecelem_magic(op)) >= 0) ylikely {
	    rs = SR_OK ;
	    if (op->va) ylikely {
	        c = op->c ;
		op->c = 0 ;
	        op->i = 0 ;
	        op->fi = 0 ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecelem_delall) */

int vecelem_sort(vecelem *op,vecelem_vcmp vcf) noex {
	int		rs ;
	if ((rs = vecelem_magic(op,vcf)) >= 0) ylikely {
	    rs = op->c ; 
	    if (! op->fl.issorted) {
	        op->fl.issorted = true ;
	        if (op->c > 1) {
		    qsort_f	scf = qsort_f(vcf) ;
		    csize	esize = size_t(op->esz) ;
		    csize	elen = size_t(op->i) ;
	            qsort(op->va,elen,esize,scf) ;
		} /* end if (sorting needed) */
	    } /* end if (not already sorted) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecelem_sort) */

int vecelem_setsorted(vecelem *op) noex {
	int		rs ;
	if ((rs = vecelem_magic(op)) >= 0) ylikely {
	    rs = op->c ; 
	    op->fl.issorted = true ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecelem_setsorted) */

int vecelem_getvec(vecelem *op,void **rpp) noex {
	int		rs ;
	if ((rs = vecelem_magic(op,rpp)) >= 0) ylikely {
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
	if ((rs = vecelem_magic(op)) >= 0) ylikely {
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
		            caddr_t cap = caddr_t(op->va) ;
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
	        if ((op->i > op->n) || (op->c > op->i)) {
		    rs = SR_BADFMT ;
		}
	    } /* end if (ok) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecelem_audit) */


/* private subroutines */

local int vecelem_ctor(vecelem *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    op->va = nullptr ;
	    op->c = 0 ;
	    op->i = 0 ;
	    op->n = 0 ;
	    op->fi = 0 ;
	    op->esz = 0 ;
	    op->magic = 0 ;
	    op->fl = {} ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (vecelem_ctor) */

local int vecelem_dtor(vecelem *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	}
	return rs ;
} /* end subroutine (vecelem_dtor) */

consteval int mkoptmask() noex {
	int		m = 0 ;
	m |= vecelemm.reuse ;
	m |= vecelemm.compact ;
	m |= vecelemm.swap ;
	m |= vecelemm.stationary ;
	m |= vecelemm.conserve ;
	m |= vecelemm.sorted ;
	m |= vecelemm.ordered ;
	return m ;
} /* end subroutine (mkoptmask) */

local int vecelem_setopts(vecelem *op,int vo) noex {
	constexpr int	optmask = mkoptmask() ;
	int		rs = SR_INVALID ;
	if ((vo & (~ optmask)) == 0) ylikely {
	    rs = SR_OK ;
	    op->fl = {} ;
	    if (vo & vecelemm.reuse)		op->fl.oreuse		= true ;
	    if (vo & vecelemm.swap)		op->fl.oswap		= true ;
	    if (vo & vecelemm.stationary)	op->fl.ostationary	= true ;
	    if (vo & vecelemm.compact)		op->fl.ocompact		= true ;
	    if (vo & vecelemm.sorted)		op->fl.osorted		= true ;
	    if (vo & vecelemm.ordered)		op->fl.oordered		= true ;
	    if (vo & vecelemm.conserve)		op->fl.oconserve 	= true ;
	} /* end if (valid) */
	return rs ;
} /* end subroutine (vecelem_setopts) */

local int vecelem_extend(vecelem *op) noex {
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
	    } /* end if (ok) */
	} /* end if (extension required) */
	return rs ;
} /* end subroutine (vecelem_extend) */

local int vecelem_validx(vecelem *op,int i) noex {
	return ((i >= 0) && (i < op->i)) ? SR_OK : SR_NOENT ;
} /* end subroutine (vecelem_validx) */



/* vechand SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* vector list operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	vechand

	Description:
	This object is used when the caller just wants to store
	their own pointer in a vector.  These routines will not
	copy the structure pointed to by the passed pointer.  The
	caller is responsible for keeping the original data in scope
	during the whole life span of the vector list.

	Options:
	reuse		= reuse empty slots
	compact		= do not allow for holes
	swap		= use swapping for empty slot management
	stationary	= entries do not move
	conserve	= conserve space where possible
	sorted		= maintain a sorted list
	ordered		= maintain an ordered list

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<localmisc.h>

#include	"vechand.h"


/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */

#ifndef	TYPEDEF_CV
#define	TYPEDEF_CV
typedef const void	cv ;
#endif

extern "C" {
    typedef int (*qsort_f)(cvoid *,cvoid *) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	vechand_ctor(vechand *) noex ;
static int	vechand_setopts(vechand *,int) noex ;
static int	vechand_extend(vechand *) noex ;
static int	vechand_validx(vechand *,int) noex ;

static int	mkoptmask() noex ;


/* local subroutines */


/* local variables */

cint			defents = VECHAND_DEFENTS ;

static cint		optmask = mkoptmask() ;


/* exported variables */

int vechandms::reuse		= (1 << vechando_reuse) ;
int vechandms::compact		= (1 << vechando_compact) ;
int vechandms::swap		= (1 << vechando_swap) ;
int vechandms::stationary	= (1 << vechando_stationary) ;
int vechandms::conserve		= (1 << vechando_conserve) ;
int vechandms::sorted		= (1 << vechando_sorted) ;
int vechandms::ordered		= (1 << vechando_ordered) ;

const vechandms		vechandm ;


/* exported subroutines */

int vechand_start(vechand *op,int vn,int vo) noex {
	int		rs ;
	if ((rs = vechand_ctor(op)) >= 0) {
	    if (vn <= 1) vn = defents ;
	    if ((rs = vechand_setopts(op,vo)) >= 0) {
	        cint	sz = (vn + 1) * szof(cvoid **) ;
	        if (void **va ; (rs = libmem.mall(sz,&va)) >= 0) {
		    op->va = va ;
	            op->va[0] = nullptr ;
	            op->n = vn ;
	            op->fl.issorted = false ;
	        } /* end if (memory-allocation) */
	    } /* end if (options) */
	} /* end if (non-null) */
	return (rs >= 0) ? vn : rs ;
}
/* end subroutine (vechand_start) */

int vechand_finish(vechand *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->va) {
	        rs1 = libmem.free(op->va) ;
	        if (rs >= 0) rs = rs1 ;
	        op->va = nullptr ;
	        op->c = 0 ;
	        op->i = 0 ;
	        op->n = 0 ;
	    } /* end if (populated) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vechand_finish) */

int vechand_add(vechand *op,cvoid *nep) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && nep) {
	    if ((rs = vechand_extend(op)) >= 0) {
	        bool	f_done = false ;
	        bool	f ;
	        f = (op->fl.oreuse || op->fl.oconserve) && (! op->fl.oordered) ;
	        if (f && (op->c < op->i)) {
	            i = op->fi ;
	            while ((i < op->i) && (op->va[i] != nullptr)) {
	                i += 1 ;
	            }
	            if (i < op->i) {
	                op->va[i] = voidp(nep) ;
	                op->fi = i + 1 ;
	                f_done = true ;
	            } else {
	                op->fi = i ;
	            }
	        } /* end if (possible reuse strategy) */
	        if (! f_done) {
	            if ((op->i + 1) > op->n) {
	                rs = vechand_extend(op) ;
	            }
	            if (rs >= 0) {
	                i = op->i ;
	                op->va[op->i++] = voidp(nep) ;
	                op->va[op->i] = nullptr ;
	            }
	        } /* end if (added elsewhere) */
	        if (rs >= 0) op->c += 1 ;	/* increment list count */
	        op->fl.issorted = false ;
	    } /* end if (extended) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vechand_add) */

int vechand_get(vechand *op,int i,void **rpp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOENT ;
	    if (op->va) {
		rs = vechand_validx(op,i) ;
	        if (rpp) {
	            *rpp = (rs >= 0) ? op->va[i] : nullptr ;
	        }
	    } /* end if (populated) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vechand_get) */

int vechand_getlast(vechand *op,void **rpp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op) {
	    rs = SR_NOENT ;
	    if (op->va) {
	        if (op->c > 0) {
		    rs = SR_OK ;
	            i = (op->i-1) ;
	            while ((i >= 0) && (op->va[i] == nullptr)) {
		        i -= 1 ;
	            }
	            if (i < 0) rs = SR_BUGCHECK ;
	        } /* end if (possible) */
	        if (rpp) {
	            *rpp = (rs >= 0) ? op->va[i] : nullptr ;
	        }
	    } /* end if (populated) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vechand_getlast) */

int vechand_search(vechand *op,cv *ep,vechand_vcmp vcf,void **rpp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && ep && vcf) {
	    rs = SR_NOENT ;
	    if (op->va) {
		csize	nsize = size_t(op->i) ;
		csize	esize = szof(void *) ;
	        if (op->fl.osorted && (! op->fl.issorted)) {
	            op->fl.issorted = true ;
	            if (op->c > 1) {
			qsort_f		scf = qsort_f(vcf) ;
		        qsort(op->va,nsize,esize,scf) ;
	            }
	        }
	        if (op->fl.issorted) {
		    qsort_f	scf = qsort_f(vcf) ;
	            void	**spp ;
	            spp = (void **) bsearch(&ep,op->va,nsize,esize,scf) ;
	            rs = SR_NOTFOUND ;
	            if (spp) {
	                i = intconv(spp - op->va) ;
	                rs = SR_OK ;
	            }
	        } else {
	            for (i = 0 ; i < op->i ; i += 1) {
	                cvoid	*lep = cvoidp(op->va[i]) ;
	                if (lep) {
	                    if ((*vcf)(&ep,&lep) == 0) break ;
		        }
	            } /* end for */
	            rs = (i < op->i) ? SR_OK : SR_NOTFOUND ;
	        } /* end if */
	        if (rpp) {
	            *rpp = (rs >= 0) ? op->va[i] : nullptr ;
	        }
	    } /* end if (populated) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vechand_search) */

int vechand_ent(vechand *op,cvoid *vp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && vp) {
	    rs = SR_NOENT ;
	    if (op->va) {
	        for (i = 0 ; i < op->i ; i += 1) {
	            if (op->va[i]) {
	                if (op->va[i] == vp) break ;
	            }
	        } /* end for */
	        if (i == op->i) rs = SR_NOTFOUND ;
	    } /* end if (populated) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vechand_ent) */

int vechand_del(vechand *op,int i) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->va) {
		if ((rs = vechand_validx(op,i)) >= 0) {
	            bool	f_fi = false ;
		    op->c -= 1 ;
		    if (op->fl.ostationary) {
	    		op->va[i] = nullptr ;
	    		if (i == (op->i - 1)) op->i -= 1 ;
	    		f_fi = true ;
		    } else if (op->fl.issorted || op->fl.oordered) {
	                if (op->fl.ocompact) {
		            int	j ;
	                    op->i -= 1 ;
	                    for (j = i ; j < op->i ; j += 1) {
	                        op->va[j] = op->va[j + 1] ;
		            }
	                    op->va[op->i] = nullptr ;
	                } else {
	                    op->va[i] = nullptr ;
	                    if (i == (op->i - 1)) op->i -= 1 ;
	                    f_fi = true ;
	                } /* end if */
	            } else {
			bool	f = (op->fl.oswap || op->fl.ocompact)  ;
			f = f && (i < (op->i - 1)) ;
			if (f) {
	                    op->va[i] = op->va[op->i - 1] ;
	                    op->va[--op->i] = nullptr ;
	                    op->fl.issorted = false ;
	                } else {
	                    op->va[i] = nullptr ;
	                    if (i == (op->i - 1)) op->i -= 1 ;
	                    f_fi = true ;
	                } /* end if */
	            } /* end if */
	            if (op->fl.oconserve) {
	                while (op->i > i) {
	                    if (op->va[op->i - 1] != nullptr) break ;
	                    op->i -= 1 ;
	                } /* end while */
	            } /* end if */
	            if (f_fi && (i < op->fi)) op->fi = i ;
		} /* end if (valid index) */
	    } /* end if (popularion) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vechand_del) */

int vechand_delhand(vechand *op,cvoid *ep) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = vechand_ent(op,ep)) >= 0) {
	        rs = vechand_del(op,rs) ;
	    }
	}
	return rs ;
}
/* end subroutine (vechand_delhand) */

int vechand_delall(vechand *op) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->va) {
	        c = op->c ;
		op->c = 0 ;
	        op->i = 0 ;
	        op->fi = 0 ;
	        op->va[0] = nullptr ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vechand_delall) */

int vechand_count(vechand *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->va) {
		rs = op->c ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vechand_count) */

int vechand_sort(vechand *op,vechand_vcmp vcf) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op && vcf) {
	    rs = SR_OK ;
	    if (op->va) {
		c = op->c ;
	        if (! op->fl.issorted) {
	            op->fl.issorted = true ;
	            if (op->c > 1) {
			csize		nsize = size_t(op->i) ;
			csize		esize = szof(void *) ;
			qsort_f		scf = qsort_f(vcf) ;
	                qsort(op->va,nsize,esize,scf) ;
		    }
	        } /* end if (not sorted) */
	    } /* end if (had entries) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vechand_sort) */

int vechand_issorted(vechand *op) noex {
	int		rs = SR_FAULT ;
	int		f_issorted = false ;
	if (op) {
	    rs = SR_OK ;
	    f_issorted = op->fl.issorted ;
	} /* end if (non-null) */
	return (rs >= 0) ? f_issorted : rs ;
}
/* end subroutine (vechand_issorted) */

int vechand_setsorted(vechand *op) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) {
	    rs = SR_OK ;
	    op->fl.issorted = true ;
	    c = op->c ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vechand_setsorted) */

int vechand_getvec(vechand *op,void *rp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && rp) {
	    if ((rs = vechand_extend(op)) >= 0) {
	        void	**rpp = voidpp(rp) ;
	        *rpp = voidp(op->va) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vechand_getvec) */

int vechand_extent(vechand *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->n ;
	}
	return rs ;
}
/* end subroutine (vechand_extent) */

int vechand_audit(vechand *op) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->va) {
	        for (int i = 0 ; i < op->i ; i += 1) {
	            if (op->va[i] != nullptr) {
	                int	*ip = (int *) op->va[i] ;
		        c += 1 ;
		        rs |= *ip ;		/* SEGFAULT? */
	            }
	        } /* end for */
	        rs = (c == op->c) ? SR_OK : SR_BADFMT ;
	    } /* end if (had entries) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vechand_audit) */


/* private subroutines */

static int vechand_ctor(vechand *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->va = nullptr ;
	    op->c = 0 ;
	    op->i = 0 ;
	    op->n = 0 ;
	    op->fi = 0 ;
	}
	return rs ;
}
/* end subroutine (vechand_ctor) */

static int mkoptmask() noex {
	int		m = 0 ;
	m |= vechandm.reuse ;
	m |= vechandm.compact ;
	m |= vechandm.swap ;
	m |= vechandm.stationary ;
	m |= vechandm.conserve ;
	m |= vechandm.sorted ;
	m |= vechandm.ordered ;
	return m ;
}
/* end subroutine (mkoptmask) */

static int vechand_setopts(vechand *op,int vo) noex {
	int		rs = SR_INVALID ;
	if ((vo & (~ optmask)) == 0) {
	    rs = SR_OK ;
	    op->fl = {} ;
	    if (vo & vechandm.reuse)		op->fl.oreuse = true ;
	    if (vo & vechandm.swap)		op->fl.oswap = true ;
	    if (vo & vechandm.stationary)	op->fl.ostationary = true ;
	    if (vo & vechandm.compact)		op->fl.ocompact = true ;
	    if (vo & vechandm.sorted)		op->fl.osorted = true ;
	    if (vo & vechandm.ordered)		op->fl.oordered = true ;
	    if (vo & vechandm.conserve)		op->fl.oconserve = true ;
	} /* end if (valid options) */
	return rs ;
}
/* end subroutine (vechand_setopts) */

static int vechand_extend(vechand *op) noex {
	int		rs = SR_OK ;
	if ((op->i + 1) > op->n) {
	    int		nn ;
	    int		sz ;
	    void	*nva ;
	    if (op->va == nullptr) {
	        nn = defents ;
	        sz = (nn + 1) * szof(void **) ;
	        rs = libmem.mall(sz,&nva) ;
	    } else {
	        nn = (op->n + 1) * 2 ;
	        sz = (nn + 1) * szof(void **) ;
	        rs = libmem.rall(op->va,sz,&nva) ;
	        op->va = nullptr ;
	    }
	    if (rs >= 0) {
	        op->va = voidpp(nva) ;
	        op->n = nn ;
		op->va[op->i] = nullptr ;
	    }
	} /* end if (extension required) */
	return rs ;
}
/* end subroutine (vechand_extend) */

static int vechand_validx(vechand *op,int i) noex {
	cint	rs = ((i >= 0) && (i < op->i)) ? SR_OK : SR_NOENT ;
	return rs ;
}
/* end subroutine (vechand_validx) */

int vechand::start(int an,int ao) noex {
	return vechand_start(this,an,ao) ;
}

int vechand::add(cvoid *ep) noex {
	return vechand_add(this,ep) ;
}

int vechand::getvec(void *vap) noex {
	return vechand_getvec(this,vap) ;
}

int vechand::sort(vechand_f vcmp) noex {
	return vechand_sort(this,vcmp) ;
}

int vechand::del(int ai) noex {
	if (ai < 0) ai = 0 ;
	return vechand_del(this,ai) ;
}

int vechand::delhand(cvoid *ep) noex {
    	return vechand_delhand(this,ep) ;
}

int vechand::search(cvoid *ep,vechand_f vcmp,void **rpp) noex {
    	return vechand_search(this,ep,vcmp,rpp) ;
}

void vechand::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("vechand",rs,"fini-finish") ;
	}
}

vechand::operator int () noex {
    	int		rs = SR_NOTOPEN ;
	if (n > 0) {
	    rs = c ;
	}
	return rs ;
}

vechand_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case vechandmem_count:
	        rs = vechand_count(op) ;
	        break ;
	    case vechandmem_setsorted:
	        rs = vechand_setsorted(op) ;
	        break ;
	    case vechandmem_issorted:
	        rs = vechand_issorted(op) ;
	        break ;
	    case vechandmem_delall:
	        rs = vechand_delall(op) ;
	        break ;
	    case vechandmem_extent:
	        rs = vechand_extent(op) ;
	        break ;
	    case vechandmem_audit:
	        rs = vechand_audit(op) ;
	        break ;
	    case vechandmem_finish:
	        rs = vechand_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (vechand_co::operator) */



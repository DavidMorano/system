/* vechand SUPPORT */
/* encoding=ISO8859-1 */
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

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

#include	"vechand.h"


/* local defines */


/* imported namespaces */


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


/* local subroutines */

consteval int mkoptmask() noex {
	int		m = 0 ;
	m |= VECHAND_OREUSE ;
	m |= VECHAND_OCOMPACT ;
	m |= VECHAND_OSWAP ;
	m |= VECHAND_OSTATIONARY ;
	m |= VECHAND_OCONSERVE ;
	m |= VECHAND_OSORTED ;
	m |= VECHAND_OORDERED ;
	return m ;
}
/* end subroutine (mkoptmask) */


/* local variables */

constexpr int		optmask = mkoptmask() ;


/* exported variables */


/* exported subroutines */

int vechand_start(vechand *op,int n,int opts) noex {
	int		rs ;
	if ((rs = vechand_ctor(op)) >= 0) {
	    if (n <= 1) n = VECHAND_DEFENTS ;
	    if ((rs = vechand_setopts(op,opts)) >= 0) {
	        cint	size = (n + 1) * szof(cvoid **) ;
	        void	**va ;
	        if ((rs = uc_libmalloc(size,&va)) >= 0) {
		    op->va = va ;
	            op->va[0] = nullptr ;
	            op->n = n ;
	            op->f.issorted = false ;
	        } /* end if (memory-allocation) */
	    } /* end if (options) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (vechand_start) */

int vechand_finish(vechand *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->va) {
	        rs1 = uc_libfree(op->va) ;
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
	        bool		f_done = false ;
	        bool		f ;
	        f = (op->f.oreuse || op->f.oconserve) && (! op->f.oordered) ;
	        if (f && (op->c < op->i)) {
	            i = op->fi ;
	            while ((i < op->i) && (op->va[i] != nullptr)) {
	                i += 1 ;
	            }
	            if (i < op->i) {
	                op->va[i] = (void *) nep ;
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
	                op->va[op->i++] = (void *) nep ;
	                op->va[op->i] = nullptr ;
	            }
	        } /* end if (added elsewhere) */
	        if (rs >= 0) op->c += 1 ;	/* increment list count */
	        op->f.issorted = false ;
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
		csize	esize = szof(void *) ;
	        if (op->f.osorted && (! op->f.issorted)) {
	            op->f.issorted = true ;
	            if (op->c > 1) {
			qsort_f		scf = qsort_f(vcf) ;
		        qsort(op->va,op->i,esize,scf) ;
	            }
	        }
	        if (op->f.issorted) {
		    qsort_f	scf = qsort_f(vcf) ;
	            void	**spp ;
	            spp = (void **) bsearch(&ep,op->va,op->i,esize,scf) ;
	            rs = SR_NOTFOUND ;
	            if (spp) {
	                i = (spp - op->va) ;
	                rs = SR_OK ;
	            }
	        } else {
	            for (i = 0 ; i < op->i ; i += 1) {
	                cvoid	*lep = (cvoid *) op->va[i] ;
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
		    if (op->f.ostationary) {
	    		op->va[i] = nullptr ;
	    		if (i == (op->i - 1)) op->i -= 1 ;
	    		f_fi = true ;
		    } else if (op->f.issorted || op->f.oordered) {
	                if (op->f.ocompact) {
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
			bool	f = (op->f.oswap || op->f.ocompact)  ;
			f = f && (i < (op->i - 1)) ;
			if (f) {
	                    op->va[i] = op->va[op->i - 1] ;
	                    op->va[--op->i] = nullptr ;
	                    op->f.issorted = false ;
	                } else {
	                    op->va[i] = nullptr ;
	                    if (i == (op->i - 1)) op->i -= 1 ;
	                    f_fi = true ;
	                } /* end if */
	            } /* end if */
	            if (op->f.oconserve) {
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
	        if (! op->f.issorted) {
	            op->f.issorted = true ;
	            if (op->c > 1) {
			csize		esize = szof(void *) ;
			qsort_f		scf = qsort_f(vcf) ;
	                qsort(op->va,op->i,esize,scf) ;
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
	    f_issorted = op->f.issorted ;
	} /* end if (non-null) */
	return (rs >= 0) ? f_issorted : rs ;
}
/* end subroutine (vechand_issorted) */

int vechand_setsorted(vechand *op) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) {
	    rs = SR_OK ;
	    op->f.issorted = true ;
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
	        void	**rpp = (void **) rp ;
	        *rpp = (void *) op->va ;
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

static int vechand_setopts(vechand *op,int vo) noex {
	int		rs = SR_INVALID ;
	if ((vo & (~optmask)) == 0) {
	    rs = SR_OK ;
	    op->f = {} ;
	    if (vo & VECHAND_OREUSE) op->f.oreuse = 1 ;
	    if (vo & VECHAND_OSWAP) op->f.oswap = 1 ;
	    if (vo & VECHAND_OSTATIONARY) op->f.ostationary = 1 ;
	    if (vo & VECHAND_OCOMPACT) op->f.ocompact = 1 ;
	    if (vo & VECHAND_OSORTED) op->f.osorted = 1 ;
	    if (vo & VECHAND_OORDERED) op->f.oordered = 1 ;
	    if (vo & VECHAND_OCONSERVE) op->f.oconserve = 1 ;
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
	        nn = VECHAND_DEFENTS ;
	        sz = (nn + 1) * szof(void **) ;
	        rs = uc_libmalloc(sz,&nva) ;
	    } else {
	        nn = (op->n + 1) * 2 ;
	        sz = (nn + 1) * szof(void **) ;
	        rs = uc_librealloc(op->va,sz,&nva) ;
	        op->va = nullptr ;
	    }
	    if (rs >= 0) {
	        op->va = (void **) nva ;
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



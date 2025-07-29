/* recarr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* record-array management */
/* version %I% last-modified %G% */

#define	CF_QSORT	0		/* use |qsort(3c)| */

/* revision history:

	= 1998-12-01, David A­D­ Morano
	This module was originally written for hardware CAD support.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object is used when the caller just wants to store
	their own pointer in a vector.  These routines will not
	copy the structure pointed to by the passed pointer.  The
	caller is responsible for keeping the original data in scope
	during the whole life span of the record-array.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |sort(3c++)| */
#include	<usystem.h>
#include	<localmisc.h>

#include	"recarr.h"


/* local defines */

#define	RECARR_DEFENTS	10

#ifdef	CF_QSORT
#define	CF_QSORT	0		/* use |qsort(3c)| */
#endif


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef int (*qsort_f)(cvoid *,cvoid *) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int recarr_ctor(recarr *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_OK ;
	    op->va = nullptr ;
	    op->c = 0 ;
	    op->i = 0 ;
	    op->n = 0 ;
	    op->fi = 0 ;
	    op->fl = {} ;
	}
	return rs ;
} /* end subroutine (recarr_ctor) */

static int recarr_dtor(recarr *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	}
	return rs ;
} /* end subroutine (recarr_dtor) */

static int	recarr_setopts(recarr *,int) noex ;
static int	recarr_extend(recarr *,int = 0) noex ;


/* local subroutines */


/* local variables */

constexpr bool		f_qsort = CF_QSORT ;


/* exported variables */


/* exported subroutines */

int recarr_start(recarr *op,int n,int opts) noex {
	int		rs ;
	if ((rs = recarr_ctor(op)) >= 0) {
	    if (n <= 1) n = RECARR_DEFENTS ;
	    if ((rs = recarr_setopts(op,opts)) >= 0) {
	        cint	sz = (n + 1) * szof(void **) ;
	        if (void *vp ; (rs = uc_libmalloc(sz,&vp)) >= 0) {
		    op->va = voidpp(vp) ;
		    op->n = n ;
	            op->va[0] = nullptr ;
	        } /* end if (memory-allocation) */
	    } /* end if (recarr_setopts) */
	    if (rs < 0) {
		recarr_dtor(op) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (recarr_start) */

int recarr_finish(recarr *op) noex {
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
	    {
		rs1 = recarr_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (recarr_finish) */

int recarr_add(recarr *op,cvoid *sp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && sp) {
	    bool	f_done = false ;
	    bool 	f = true ;
	    rs = SR_OK ;
	    f = f && (op->fl.oreuse || op->fl.oconserve) ;
	    f = f && (! op->fl.oordered) ;
	    if (f && (op->c < op->i)) {
	        i = op->fi ;
	        while ((i < op->i) && (op->va[i] != nullptr)) {
	            i += 1 ;
    		}
	        if (i < op->i) {
	            op->va[i] = (void *) sp ;
	            op->fi = i + 1 ;
	            f_done = true ;
	        } else {
	            op->fi = i ;
	        }
	    } /* end if (possible reuse strategy) */
	    if (! f_done) {
	        if ((op->i + 1) > op->n) {
	            rs = recarr_extend(op) ;
	        }
	        if (rs >= 0) {
	            i = op->i ;
	            op->va[(op->i)++] = (void *) sp ;
	            op->va[op->i] = nullptr ;
	        }
	    } /* end if (added elsewhere) */
	    if (rs >= 0) op->c += 1 ;
	    op->fl.issorted = false ;
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (recarr_add) */

int recarr_audit(recarr *op) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->va) {
	        for (int i = 0 ; i < op->i ; i += 1) {
	            if (op->va[i] != nullptr) {
	                int	*ip = (int *) op->va[i] ;
		        rs |= *ip ;		/* SEGFAULT? */
	            }
	        } /* end for */
	        rs = (c == op->c) ? SR_OK : SR_BADFMT ;
	    } /* end if (populated) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (recarr_audit) */

int recarr_get(recarr *op,int i,cvoid *vp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    void	**epp = (void **) vp ;
	    rs = SR_NOTFOUND ;
	    if (op->va) {
	        if ((i >= 0) && (i < op->i)) {
		    rs = SR_OK ;
	            if (epp) {
	                *epp = op->va[i] ;
	            }
		} /* end if (valid) */
	    } /* end if (populated) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (recarr_get) */

int recarr_getlast(recarr *op,cvoid *vp) noex {
	void		**epp = (void **) vp ;
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op) {
	    rs = SR_NOTFOUND ;
	    if (op->va) {
	        if ((op->c > 0) && (op->i > 0)) {
		    rs = SR_OK ;
	            i = (op->i-1) ;
	            while ((i >= 0) && (op->va[i] == nullptr)) {
		        i -= 1 ;
	            }
	        } 
	    } /* end if (populated) */
	} /* end if (non-null) */
	if (epp) {
	    *epp = (rs >= 0) ? op->va[i] : nullptr ;
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (recarr_getlast) */

int recarr_ent(recarr *op,cvoid *ep) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && ep) {
	    rs = SR_NOTFOUND ;
	    if (op->va) {
	        for (i = 0 ; i < op->i ; i += 1) {
	            if (op->va[i]) {
	                if (op->va[i] == ep) break ;
		    }
	        } /* end for */
	        if (i < op->i) rs = SR_OK ;
	    } /* end if (populated) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (recarr_ent) */

int recarr_del(recarr *op,int i) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) {
	    rs = SR_NOTFOUND ;
	    if ((i >= 0) && (i < op->i)) {
	        if (op->va) {
	            bool	f_fi = false ;
	            op->c -= 1 ;
	            if (op->fl.ostationary) {
	                op->va[i] = nullptr ;
	                if (i == (op->i - 1)) op->i -= 1 ;
	                f_fi = true ;
	            } else if (op->fl.issorted || op->fl.oordered) {
	                if (op->fl.ocompact) {
	                    op->i -= 1 ;
	                    for (int j = i ; j < op->i ; j += 1) {
	                        op->va[j] = op->va[j + 1] ;
		            }
	                    op->va[op->i] = nullptr ;
	                } else {
	                    op->va[i] = nullptr ;
	                    if (i == (op->i - 1)) op->i -= 1 ;
	                    f_fi = true ;
	                } /* end if */
	            } else {
			bool	f = true ;
	                f = f && (op->fl.oswap || op->fl.ocompact) ;
			f = f && (i < (op->i-1)) ;
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
	            if (f_fi && (i < op->fi)) {
	                op->fi = i ;
	            }
	            rs = op->c ; /* <- return-status set */
	        } /* end if (populated) */
	    } /* end if (valid index) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (recarr_del) */

int recarr_delhand(recarr *op,cvoid *ep) noex {
	int		rs ;
	if ((rs = recarr_ent(op,ep)) >= 0) {
	    rs = recarr_del(op,rs) ;
	}
	return rs ;
}
/* end subroutine (recarr_delhand) */

int recarr_delall(recarr *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->va) {
		rs = op->c ;
	        op->i = 0 ;
	        op->fi = 0 ;
	        op->va[0] = nullptr ;
	        op->c = 0 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (recarr_delall) */

int recarr_count(recarr *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->c ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (recarr_count) */

int recarr_sort(recarr *op,recarr_cf vcmp) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op && vcmp) {
	    rs = SR_OK ;
	    if (op->va) {
		c = op->c ;
	        if (! op->fl.issorted) {
	            op->fl.issorted = true ;
	            if (op->c > 1) {
			qsort_f		qcmp = qsort_f(vcmp) ;
			if_constexpr (f_qsort) {
			    cint	esize = szof(void *) ;
	                    qsort(op->va,op->i,esize,qcmp) ;
			} else {
			    std::sort(op->va,(op->va+op->i),qcmp) ;
			}
	            }
	        } /* end if (sorting needed) */
	    } /* end if (populated) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (recarr_sort) */

int recarr_setsorted(recarr *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->c ;
	    op->fl.issorted = true ;
	}
	return rs ;
}
/* end subroutine (recarr_setsorted) */

int recarr_search(recarr *op,cvoid *ep,recarr_cf vcmp,void *vrp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	void		**rpp = (void **) vrp ;
	if (op && ep && vcmp) {
	    cint	esize = szof(void *) ;
	    void	**spp{} ;
	    if (op->fl.osorted && (! op->fl.issorted)) {
	        op->fl.issorted = true ;
	        if (op->c > 1) {
		    qsort_f	qcmp = qsort_f(vcmp) ;
		    qsort(op->va,op->i,esize,qcmp) ;
	        }
	    }
	    if (op->fl.issorted) {
		qsort_f		qcmp = qsort_f(vcmp) ;
	        spp = (void **) bsearch(&ep,op->va,op->i,esize,qcmp) ;
	        rs = SR_NOTFOUND ;
	        if (spp) {
	            i = intconv(spp - op->va) ;
	            rs = SR_OK ;
	        }
	    } else {
	        for (i = 0 ; i < op->i ; i += 1) {
	            cvoid	*lep = op->va[i] ;
	            if (lep) {
	                if ((*vcmp)(&ep,&lep) == 0) break ;
		    }
	        } /* end for */
	        rs = (i < op->i) ? SR_OK : SR_NOTFOUND ;
	    } /* end if */
	} /* end if (non-null) */
	if (rpp) {
	    *rpp = (rs >= 0) ? op->va[i] : nullptr ;
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (recarr_search) */

int recarr_getvec(recarr *op,void *rp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && rp) {
	    void	**rpp = (void **) rp ;
	    if ((rs = recarr_extend(op,1)) >= 0) {
	        *rpp = (void *) op->va ;
	    }
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (recarr_getvec) */

int recarr_extent(recarr *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->n ;
	}
	return rs ;
}
/* end subroutine (recarr_extent) */


/* private subroutines */

consteval int mkoptmask() noex {
	int		m = 0 ;
	m |= RECARR_OREUSE ;
	m |= RECARR_OCOMPACT ;
	m |= RECARR_OSWAP ;
	m |= RECARR_OSTATIONARY ;
	m |= RECARR_OCONSERVE ;
	m |= RECARR_OSORTED ;
	m |= RECARR_OORDERED ;
	return m ;
}
/* end subroutine (mkoptmask) */

static int recarr_setopts(recarr *op,int vo) noex {
	constexpr int	optmask = mkoptmask() ;
	int		rs = SR_INVALID ;
	if ((vo & (~ optmask)) == 0) {
	    rs = SR_OK ;
	    op->fl = {} ;
	    if (vo & RECARR_OREUSE) op->fl.oreuse = 1 ;
	    if (vo & RECARR_OCOMPACT) op->fl.ocompact = 1 ;
	    if (vo & RECARR_OSWAP) op->fl.oswap = 1 ;
	    if (vo & RECARR_OSTATIONARY) op->fl.ostationary = 1 ;
	    if (vo & RECARR_OCONSERVE) op->fl.oconserve = 1 ;
	    if (vo & RECARR_OSORTED) op->fl.osorted = 1 ;
	    if (vo & RECARR_OORDERED) op->fl.oordered = 1 ;
	} /* end if (valid options) */
	return rs ;
}
/* end subroutine (recarr_setopts) */

static int recarr_extend(recarr *op,int n) noex {
	int		rs = SR_OK ;
	if ((op->i + 1) > op->n) {
	    int		nn ;
	    int		sz ;
	    void	*nva{} ;
	    if (op->va == nullptr) {
	        nn = (n) ? n : RECARR_DEFENTS ;
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
	} /* end if */
	return rs ;
}
/* end subroutine (recarr_extend) */



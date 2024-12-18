/* vecint SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* vector integer operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

  	name:
	vecint

	Description:
	These routines are used when the caller wants to store a
	COPY of the passed element data into a vector.  These
	routines will copy and store the copied data in the list.
	The advantage is that the caller does not have to keep the
	orginal data around in order for the list data to be accessed
	later.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<localmisc.h>

#include	"vecint.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

extern "C" {
    typedef int (*qsortcmp_f)(cvoid *,cvoid *) noex ;
}


/* forward references */

template<typename ... Args>
static inline int vecint_magic(vecint *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == VECINT_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (vecint_magic) */

static int	vecint_addval(vecint *op,VECINT_TYPE) noex ;
static int	vecint_extend(vecint *,int) noex ;
static int	vecint_setopts(vecint *,int) noex ;
static int	vecint_insertval(vecint *,int,VECINT_TYPE) noex ;
static int	vecint_extrange(vecint *,int) noex ;

static int	deftypecmp(const VECINT_TYPE *,const VECINT_TYPE *) noex ;

consteval int mkoptmask() noex {
	int		m = 0 ;
	m |= VECINT_OREUSE ;
	m |= VECINT_OSWAP ;
	m |= VECINT_OSTATIONARY ;
	m |= VECINT_OCOMPACT ;
	m |= VECINT_OSORTED ;
	m |= VECINT_OORDERED ;
	m |= VECINT_OCONSERVE ;
	return m ;
}
/* end subroutine (mkoptmask) */


/* local variables */


/* exported variables */


/* exported subroutines */

int vecint_start(vecint *op,int vn,int vo) noex {
    	VECINT		*hop = op ;
	int		rs = SR_FAULT ;
	if (vn < 0) vn = VECINT_DEFENTS ;
	if (op) {
	    memclear(hop) ;
	    if ((rs = vecint_setopts(op,vo)) >= 0) {
	        op->n = vn ;
	        if (vn > 0) {
	            cint	sz = (vn + 1) * szof(VECINT_TYPE) ;
		    if (void *vp{} ; (rs = uc_malloc(sz,&vp)) >= 0) {
		        op->va = (VECINT_TYPE *) vp ;
	    	        op->va[0] = INT_MIN ;
		    }
	        } /* end if (wanted pre-allocation) */
	        if (rs >= 0) {
		    op->magic = VECINT_MAGIC ;
	        }
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? vn : rs ;
}
/* end subroutine (vecint_start) */

int vecint_finish(vecint *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = vecint_magic(op)) >= 0) {
	    if (op->va) {
	        rs1 = uc_free(op->va) ;
	        if (rs >= 0) rs = rs1 ;
	        op->va = nullptr ;
	    }
	    op->c = 0 ;
	    op->i = 0 ;
	    op->n = 0 ;
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_finish) */

int vecint_add(vecint *op,VECINT_TYPE v) noex {
	int		rs ;
	if ((rs = vecint_magic(op)) >= 0) {
	    rs = vecint_addval(op,v) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_add) */

extern int vecint_addlist(vecint *op,const VECINT_TYPE *lp,int ll) noex {
	int		rs ;
	if ((rs = vecint_magic(op,lp)) >= 0) {
	    for (int i = 0 ; (rs >= 0) && (i < ll) ; i += 1) {
	        rs = vecint_addval(op,lp[i]) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_addlist) */

int vecint_adduniq(vecint *op,VECINT_TYPE v) noex {
	int		rs ;
	if ((rs = vecint_magic(op)) >= 0) {
	    int		i = 0 ;
	    rs = INT_MAX ;
	    for (i = 0 ; i < op->i ; i += 1) {
	        if (op->va[i] == v) break ;
	    } /* end for */
	    if (i >= op->i) {
	        rs = vecint_addval(op,v) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_adduniq) */

int vecint_insert(vecint *op,int ii,VECINT_TYPE val) noex {
	int		rs ;
	if ((rs = vecint_magic(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (ii >= 0) {
		rs = SR_OK ;
	        if ((ii+1) > op->n) {
	            rs = vecint_extend(op,((ii+1)-op->n)) ;
	        }
	        if (rs >= 0) {
		    if ((rs = vecint_extrange(op,(ii+1))) >= 0) {
	                rs = vecint_insertval(op,ii,val) ;
		    }
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_insert) */

int vecint_assign(vecint *op,int ii,VECINT_TYPE val) noex {
	int		rs ;
	if ((rs = vecint_magic(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (ii >= 0) {
		rs = SR_OK ;
	        if ((ii+1) > op->n) {
	            rs = vecint_extend(op,((ii+1)-op->n)) ;
	        }
	        if (rs >= 0) {
		    if ((rs = vecint_extrange(op,(ii+1))) >= 0) {
	                op->va[ii] = val ;
		        op->va[op->i] = INT_MAX ;
	            }
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_assign) */

int vecint_resize(vecint *op,int n) noex {
	int		rs ;
	if ((rs = vecint_magic(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (n >= 0) {
		rs = SR_OK ;
	        if (n != op->i) {
	            if (n > op->n) {
	                rs = vecint_extend(op,(n-op->n)) ;
	            }
	            if (rs >= 0) {
		        if ((rs = vecint_extrange(op,n)) >= 0) {
		            if (n < op->i) {
			        op->i = n ;
		            }
		            op->c = n ;
		            op->va[op->i] = INT_MIN ;
		        }
	            } /* end if (ok) */
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_resize) */

int vecint_getval(vecint *op,int i,VECINT_TYPE *rp) noex {
	int		rs ;
	if ((rs = vecint_magic(op)) >= 0) {
	     if ((i < 0) || (i >= op->i)) {
		rs = SR_NOTFOUND ;
	     }
	     if (rp) {
	         *rp = (rs >= 0) ? op->va[i] : INT_MIN ;
	     }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_getval) */

int vecint_del(vecint *op,int i) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = vecint_magic(op)) >= 0) {
	    rs = SR_NOTFOUND ;
	    if ((i >= 0) && (i < op->i)) {
	        bool	f_fi = false ;
		rs = SR_OK ;
		/* delete the entry */
	        op->c -= 1 ;			/* decrement list count */
		/* apply the appropriate deletion based on management policy */
	        if (op->f.ostationary) {
	            (op->va)[i] = INT_MIN ;
	            if (i == (op->i - 1)) {
	                op->i -= 1 ;
	            }
	            f_fi = true ;
	        } else if (op->f.issorted || op->f.oordered) {
	            if (op->f.ocompact) {
		        int	j ;
	                op->i -= 1 ;
	                for (j = i ; j < op->i ; j += 1) {
	                    (op->va)[j] = (op->va)[j + 1] ;
		        }
	                (op->va)[op->i] = INT_MIN ;
	            } else {
	                (op->va)[i] = INT_MIN ;
	                if (i == (op->i - 1)) {
	                    op->i -= 1 ;
		        }
	                f_fi = true ;
	            } /* end if */
	        } else {
	            if ((op->f.oswap || op->f.ocompact) && (i < (op->i - 1))) {
	                (op->va)[i] = (op->va)[op->i - 1] ;
	                (op->va)[--op->i] = INT_MIN ;
	                op->f.issorted = false ;
	            } else {
	                (op->va)[i] = INT_MIN ;
	                if (i == (op->i - 1)) {
	                    op->i -= 1 ;
		        }
	                f_fi = true ;
	            } /* end if */
	        } /* end if */
	        if (f_fi && (i < op->fi)) {
	            op->fi = i ;
	        }
	        c = op->c ;
	    } /* end if (found) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecint_del) */

int vecint_delall(vecint *op) noex {
	int		rs ;
	if ((rs = vecint_magic(op)) >= 0) {
	    op->i = 0 ;
	    op->c = 0 ;
	    op->fi = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_delall) */

int vecint_count(vecint *op) noex {
	int		rs ;
	if ((rs = vecint_magic(op)) >= 0) {
	    rs = op->c ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_count) */

int vecint_extent(vecint *op) noex {
	int		rs ;
	if ((rs = vecint_magic(op)) >= 0) {
	    rs = op->i ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_extent) */

int vecint_sort(vecint *op) noex {
	int		rs ;
	if ((rs = vecint_magic(op)) >= 0) {
	    if (! op->f.issorted) {
	        op->f.issorted = true ;
	        if (op->c > 1) {
		    cint	esize = szof(VECINT_TYPE) ;
		    qsortcmp_f	qcf = qsortcmp_f(deftypecmp) ;
	            qsort(op->va,op->i,esize,qcf) ;
	        }
	    }
	    rs = op->c ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_sort) */

int vecint_setsorted(vecint *op) noex {
	int		rs ;
	if ((rs = vecint_magic(op)) >= 0) {
	    op->f.issorted = true ;
	    rs = op->c ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_setsorted) */

int vecint_find(vecint *op,VECINT_TYPE v) noex {
	int		rs ;
	int		i = 0 ; /* � GCC false complaint */
	if ((rs = vecint_magic(op)) >= 0) {
	    if (op->f.issorted) {
	        cint		esz = szof(VECINT_TYPE) ;
	        qsortcmp_f	qcf = qsortcmp_f(deftypecmp) ;
	        int		*rpp ;
	        rpp = (int *) bsearch(&v,op->va,op->i,esz,qcf) ;
	        rs = SR_NOTFOUND ;
	        if (rpp) {
	            i = (rpp - op->va) ;
	            rs = SR_OK ;
	        }
	    } else {
	        for (i = 0 ; i < op->i ; i += 1) {
	            if (op->va[i] == v) break ;
	        } /* end for */
	        rs = (i < op->i) ? SR_OK : SR_NOTFOUND ;
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecint_find) */

int vecint_match(vecint *op,VECINT_TYPE v) noex {
	int		rs ;
	if ((rs = vecint_find(op,v)) >= 0) {
	    rs = true ;
	} else if (rs == SR_NOTFOUND) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (vecint_match) */

int vecint_getvec(vecint *op,VECINT_TYPE **rpp) noex {
	int		rs ;
	if ((rs = vecint_magic(op,rpp)) >= 0) {
	    *rpp = op->va ;
	    rs = op->i ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_getvec) */

int vecint_mkvec(vecint *op,VECINT_TYPE *va) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = vecint_magic(op,va)) >= 0) {
	    for (int i = 0 ; i < op->i ; i += 1) {
		cint	v = op->va[i] ;
		if (v != INT_MIN) {
		    va[c++] = op->va[i] ;
		}
	    } /* end for */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecint_mkvec) */

int vecint_curbegin(vecint *op,vecint_cur *curp) noex {
	int		rs ;
	if ((rs = vecint_magic(op,curp)) >= 0) {
	    curp->i = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_curend) */

int vecint_curend(vecint *op,vecint_cur *curp) noex {
	int		rs ;
	if ((rs = vecint_magic(op,curp)) >= 0) {
	    curp->i = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_curend) */

int vecint_curenum(vecint *op,vecint_cur *curp,VECINT_TYPE *rp) noex {
	int		rs ;
	int		v = 0 ;
	if ((rs = vecint_magic(op,curp)) >= 0) {
	    int		i = curp->i ;
	    if ((i >= 0) && (i < op->i)) {
	        v = (op->va)[i] ;
	        curp->i = (i+1) ;
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if (magic) */
	if (rp) *rp = (rs >= 0) ? v : INT_MIN ;
	return rs ;
}
/* end subroutine (vecint_curenum) */

int vecint_audit(vecint *op) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = vecint_magic(op)) >= 0) {
	    volatile int	dummy{} ;
	    int			i = 0 ; /* <- used afterwards */
	    for (i = 0 ; i < op->i ; i += 1) {
	        dummy = op->va[i] ;
	    }
	    (void) dummy ;
	    c = op->c ;
	    rs = (i == c) ? SR_OK : SR_BADFMT ;
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecint_audit) */


/* private subroutines */

static int vecint_setopts(vecint *op,int vo) noex {
	constexpr int	m = mkoptmask() ;
	int		rs = SR_INVALID ;
	if ((vo & (~ m)) == 0) {
	    rs = SR_OK ;
	    op->f = {} ;
	    if (vo & VECINT_OREUSE) op->f.oreuse = 1 ;
	    if (vo & VECINT_OSWAP) op->f.oswap = 1 ;
	    if (vo & VECINT_OSTATIONARY) op->f.ostationary = 1 ;
	    if (vo & VECINT_OCOMPACT) op->f.ocompact = 1 ;
	    if (vo & VECINT_OSORTED) op->f.osorted = 1 ;
	    if (vo & VECINT_OORDERED) op->f.oordered = 1 ;
	    if (vo & VECINT_OCONSERVE) op->f.oconserve = 1 ;
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (vecint_setopts) */

static int vecint_addval(vecint *op,VECINT_TYPE v) noex {
	int		rs = SR_OK ;
	int		i = 0 ; /* � GCC false complaint */
	bool		f_done = false ;
	bool		f ;
/* can we fit this new entry within the existing extent? */
	f = (op->f.oreuse || op->f.oconserve) && (! op->f.oordered) ;
	if (f && (op->c < op->i)) {
	    i = op->fi ;
	    while ((i < op->i) && (op->va[i] != INT_MIN)) {
	        i += 1 ;
	    }
	    if (i < op->i) {
	        (op->va)[i] = v ;
	        op->fi = (i + 1) ;
	        f_done = true ;
	    } else {
	        op->fi = i ;
	    }
	} /* end if (possible reuse strategy) */
/* do we have to grow the vector array? */
	if (! f_done) {
/* do we have to grow the array? */
	    if ((op->i + 1) > op->n) {
	        rs = vecint_extend(op,1) ;
	    }
/* link into the list structure */
	    if (rs >= 0) {
	        i = op->i ;
	        (op->va)[(op->i)++] = v ;
	        (op->va)[op->i] = INT_MIN ;
	    }
	} /* end if */
	if (rs >= 0) {
	    op->c += 1 ;		/* increment list count */
	    op->f.issorted = false ;
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecint_addval) */

static int vecint_extend(vecint *op,int amount) noex {
	int		rs = SR_OK ;
	if (amount > 0) {
	    cint		esize = szof(VECINT_TYPE) ;
	    int			nn ;
	    int			sz ;
	    VECINT_TYPE		*nva{} ;
	    if (op->va == nullptr) {
	        nn = max(amount,VECINT_DEFENTS) ;
	        sz = ((nn + 1) * esize) ;
	        rs = uc_malloc(sz,&nva) ;
	    } else {
	        nn = max((op->n + amount),(op->n * 2)) ;
	        sz = ((nn + 1) * esize) ;
	        rs = uc_realloc(op->va,sz,&nva) ;
	    } /* end if */
	    if (rs >= 0) {
	        op->va = nva ;
	        op->n = nn ;
		op->va[op->i] = INT_MIN ;
	    }
	}
	return rs ;
}
/* end subroutine (vecint_extend) */

static int vecint_insertval(vecint *op,int ii,VECINT_TYPE val) noex {
	if (ii < op->i) {
	    int		i ;
	    /* find */
	    for (i = (ii + 1) ; i < op->i ; i += 1) {
		if (op->va[i] == INT_MIN) break ;
	    }
	    /* management */
	    if (i == op->i) {
	        op->i += 1 ;
	        op->va[op->i] = INT_MIN ;
	    }
	    /* move-up */
	    for (int j = i ; j > ii ; j -= 1) {
		op->va[j] = op->va[j-1] ;
	    }
	} else if (ii == op->i) {
	    op->i += 1 ;
	    op->va[op->i] = INT_MIN ;
	} /* end if */
	op->va[ii] = val ;
	op->c += 1 ;
	op->f.issorted = false ;
	return ii ;
}
/* end subroutine (vecint_insertval) */

static int vecint_extrange(vecint *op,int n) noex {
	if (n > op->i) {
	    cint	nsz = ((n-op->i) * szof(VECINT_TYPE)) ;
	    memclear((op->va+op->i),nsz) ;
	    op->i = n ;
	    op->va[op->i] = INT_MIN ;
	}
	return SR_OK ;
}
/* end subroutine (vecint_extrange) */

static int deftypecmp(const VECINT_TYPE *l1p,const VECINT_TYPE *l2p) noex {
	int		rc = 0 ;
	if (l1p || l2p) {
	    rc = 1 ;
	    if (l1p) {
		rc = -1 ;
		if (l2p) {
		    cint	i1 = *l1p ;
		    cint	i2 = *l2p ;
	    	    rc = (i1 - i2) ;
		}
	    }
	}
	return rc ;
}
/* end subroutine (deftypecmp) */

int vecint_st::operator () (int vn,int vo) noex {
    	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case 0:
    	        rs = vecint_start(op,vn,vo) ;
		break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}

int vecint::add(VECINT_TYPE v) noex {
	return vecint_add(this,v) ;
}

int vecint::adduniq(VECINT_TYPE v) noex {
	return vecint_adduniq(this,v) ;
}

int vecint::insert(int i,VECINT_TYPE v) noex {
	return vecint_insert(this,i,v) ;
}

int vecint::assign(int i,VECINT_TYPE v) noex {
	return vecint_insert(this,i,v) ;
}

int vecint::del(int i) noex {
    	int		rs ;
	if (i >= 0) {
	    rs = vecint_del(this,i) ;
	} else {
	    rs = vecint_delall(this) ;
	}
	return rs ;
}

int vecint::find(VECINT_TYPE v) noex {
	return vecint_find(this,v) ;
}

int vecint::match(VECINT_TYPE v) noex {
	return vecint_match(this,v) ;
}

int vecint::getval(int i,VECINT_TYPE *rp) noex {
	return vecint_getval(this,i,rp) ;
}

int vecint::getvec(VECINT_TYPE **rpp) noex {
	return vecint_getvec(this,rpp) ;
}

int vecint::mkvec(VECINT_TYPE *va) noex {
	return vecint_mkvec(this,va) ;
}

int vecint::curbegin(vecint_cur *curp) noex {
	return vecint_curbegin(this,curp) ;
}

int vecint::curend(vecint_cur *curp) noex {
	return vecint_curend(this,curp) ;
}

int vecint::curenum(vecint_cur *curp,VECINT_TYPE *rp) noex {
	return vecint_curenum(this,curp,rp) ;
}

void vecint::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("vecint",rs,"fini-finish") ;
	}
}

int vecint_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case vecintmem_count:
	        rs = vecint_count(op) ;
	        break ;
	    case vecintmem_extent:
	        rs = vecint_extent(op) ;
	        break ;
	    case vecintmem_delall:
	        rs = vecint_delall(op) ;
	        break ;
	    case vecintmem_sort:
	        rs = vecint_sort(op) ;
	        break ;
	    case vecintmem_setsorted:
	        rs = vecint_setsorted(op) ;
	        break ;
	    case vecintmem_resize:
	        rs = vecint_resize(op,a) ;
	        break ;
	    case vecintmem_audit:
	        rs = vecint_audit(op) ;
	        break ;
	    case vecintmem_finish:
	        rs = vecint_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (vecint_co::operator) */

bool vecint_iter::operator == (const vecint_iter &oit) noex {
	return (va == oit.va) && (i == oit.i) && (ii == oit.ii) ;
}

bool vecint_iter::operator != (const vecint_iter &oit) noex {
	bool		f = false ;
	f = f || (va != oit.va) ;
	f = f || (ii != oit.ii) ;
	if (!f) {
	    f = (i < oit.i) ;
	}
	return f ;
}
/* end method (vecint_iter::operator) */

vecint_iter vecint_iter::operator + (int n) const noex {
	vecint_iter	rit(va,i,i) ;
	rit.i = ((rit.i + n) >= 0) ? (rit.i + n) : 0 ;
	return rit ;
}

vecint_iter vecint_iter::operator += (int n) noex {
	vecint_iter	rit(va,i,i) ;
	i = ((i + n) >= 0) ? (i + n) : 0 ;
	rit.i = i ;
	return rit ;
}

vecint_iter vecint_iter::operator ++ () noex { /* pre */
	increment() ;
	return (*this) ;
}

vecint_iter vecint_iter::operator ++ (int) noex { /* post */
	vecint_iter	pre(*this) ;
	increment() ;
	return pre ;
}

void vecint_iter::increment(int n) noex {
	if ((i + n) < 0) n = -i ;
	if (n != 0) {
	    i += n ;
	    while ((i < ii) && (va[i] == -1)) {
	        i += 1 ;
	    }
	}
}
/* end method (vecint_iter::increment) */




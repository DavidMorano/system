/* vecint SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* vector integer operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	vecint

	Description:
	This object is used when the caller wants to store a
	COPY of the passed element data into a vector.  These
	routines will copy and store the copied data in the list.
	The advantage is that the caller does not have to keep the
	orginal data around in order for the list data to be accessed
	later.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* |INT_MIN| + |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<localmisc.h>

#include	"vecint.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */

template<typename ... Args>
local inline int vecint_magic(vecint *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magic == VECINT_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (vecint_magic) */

local int	vecint_addval(vecint *op,VECINT_TYPE) noex ;
local int	vecint_extend(vecint *,int) noex ;
local int	vecint_setopts(vecint *,int) noex ;
local int	vecint_insertval(vecint *,int,VECINT_TYPE) noex ;
local int	vecint_extrange(vecint *,int) noex ;

local int	deftypecmp(const VECINT_TYPE *,const VECINT_TYPE *) noex ;


/* local variables */


/* exported variables */

constexpr vecintms	vecintm ;


/* exported subroutines */

int vecint_start(vecint *op,int vn,int vo) noex {
    	VECINT		*hop = op ;
	int		rs = SR_FAULT ;
	if (vn < 0) vn = VECINT_DEFENTS ;
	if (op) ylikely {
	    memclear(hop) ;
	    if ((rs = vecint_setopts(op,vo)) >= 0) ylikely {
	        op->n = vn ;
	        if (vn > 0) {
	            cint	sz = (vn + 1) * szof(VECINT_TYPE) ;
		    if (void *vp ; (rs = libmem.mall(sz,&vp)) >= 0) ylikely {
		        op->va = (VECINT_TYPE *) vp ;
	    	        op->va[0] = VECINT_MIN ;
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
	if ((rs = vecint_magic(op)) >= 0) ylikely {
	    if (op->va) ylikely {
	        rs1 = libmem.free(op->va) ;
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
	if ((rs = vecint_magic(op)) >= 0) ylikely {
	    rs = vecint_addval(op,v) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_add) */

extern int vecint_addlist(vecint *op,const VECINT_TYPE *lp,int ll) noex {
	int		rs ;
	if ((rs = vecint_magic(op,lp)) >= 0) ylikely {
	    for (int i = 0 ; (rs >= 0) && (i < ll) ; i += 1) {
	        rs = vecint_addval(op,lp[i]) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_addlist) */

int vecint_adduniq(vecint *op,VECINT_TYPE v) noex {
	int		rs ;
	int		i = 0 ;
	if ((rs = vecint_magic(op)) >= 0) ylikely {
	    for (i = 0 ; i < op->i ; i += 1) {
	        if (op->va[i] == v) break ;
	    } /* end for */
	    if (i >= op->i) {
	        rs = vecint_addval(op,v) ;
		i = rs ;
	    } else {
	        i = INT_MAX ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecint_adduniq) */

int vecint_insert(vecint *op,int ii,VECINT_TYPE val) noex {
	int		rs ;
	if ((rs = vecint_magic(op)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (ii >= 0) ylikely {
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
	        if ((ii + 1) > op->n) {
		    cint n = ((ii + 1) - op->n) ;
	            rs = vecint_extend(op,n) ;
	        }
	        if (rs >= 0) ylikely {
		    if ((rs = vecint_extrange(op,(ii + 1))) >= 0) {
	                op->va[ii] = val ;
	            }
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_assign) */

int vecint_resize(vecint *op,int n) noex {
	int		rs ;
	if ((rs = vecint_magic(op)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (n >= 0) ylikely {
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
		            op->va[op->i] = VECINT_MIN ;
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
	if ((rs = vecint_magic(op)) >= 0) ylikely {
	     if ((i < 0) || (i >= op->i)) {
		rs = SR_NOTFOUND ;
	     }
	     if (rp) {
	         *rp = (rs >= 0) ? op->va[i] : VECINT_MIN ;
	     }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_getval) */

int vecint_del(vecint *op,int i) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = vecint_magic(op)) >= 0) ylikely {
	    rs = SR_NOTFOUND ;
	    if ((i >= 0) && (i < op->i)) {
	        bool	f_fi = false ;
		rs = SR_OK ;
		/* delete the entry */
	        op->c -= 1 ;			/* decrement list count */
		/* apply the appropriate deletion based on management policy */
	        if (op->fl.ostationary) {
	            (op->va)[i] = VECINT_MIN ;
	            if (i == (op->i - 1)) {
	                op->i -= 1 ;
	            }
	            f_fi = true ;
	        } else if (op->fl.issorted || op->fl.oordered) {
	            if (op->fl.ocompact) {
	                op->i -= 1 ;
	                for (int j = i ; j < op->i ; j += 1) {
	                    op->va[j] = op->va[j + 1] ;
		        }
	                op->va[op->i] = VECINT_MIN ;
	            } else {
	                op->va[i] = VECINT_MIN ;
	                if (i == (op->i - 1)) {
	                    op->i -= 1 ;
		        }
	                f_fi = true ;
	            } /* end if */
	        } else {
		    cbool f = (op->fl.oswap || op->fl.ocompact) ;
	            if (f && (i < (op->i - 1))) {
	                op->va[i] = op->va[op->i - 1] ;
	                op->va[--op->i] = VECINT_MIN ;
	                op->fl.issorted = false ;
	            } else {
	                op->va[i] = VECINT_MIN ;
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
	if ((rs = vecint_magic(op)) >= 0) ylikely {
	    op->i = 0 ;
	    op->c = 0 ;
	    op->fi = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_delall) */

int vecint_count(vecint *op) noex {
	int		rs ;
	if ((rs = vecint_magic(op)) >= 0) ylikely {
	    rs = op->c ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_count) */

int vecint_extent(vecint *op) noex {
	int		rs ;
	if ((rs = vecint_magic(op)) >= 0) ylikely {
	    rs = op->i ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_extent) */

int vecint_sort(vecint *op) noex {
	int		rs ;
	if ((rs = vecint_magic(op)) >= 0) ylikely {
	    if (! op->fl.issorted) {
	        op->fl.issorted = true ;
	        if (op->c > 1) {
		    csize	alen = size_t(op->i) ;
		    csize	esize = sizeof(VECINT_TYPE) ;
		    qsort_f	qcf = qsort_f(deftypecmp) ;
	            qsort(op->va,alen,esize,qcf) ;
	        }
	    }
	    rs = op->c ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_sort) */

int vecint_setsorted(vecint *op) noex {
	int		rs ;
	if ((rs = vecint_magic(op)) >= 0) ylikely {
	    op->fl.issorted = true ;
	    rs = op->c ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_setsorted) */

int vecint_find(vecint *op,VECINT_TYPE v) noex {
	int		rs ;
	int		i = 0 ; /* ¥ GCC false complaint */
	if ((rs = vecint_magic(op)) >= 0) ylikely {
	    if (op->fl.issorted) {
	        csize		esize = szof(VECINT_TYPE) ;
		csize		elen = size_t(op->i) ;
	        qsort_f		qcf = qsort_f(deftypecmp) ;
	        VECINT_TYPE	*rpp ;
	        rpp = (VECINT_TYPE *) bsearch(&v,op->va,elen,esize,qcf) ;
	        rs = SR_NOTFOUND ;
	        if (rpp) {
	            i = intconv(rpp - op->va) ;
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
	if ((rs = vecint_magic(op,rpp)) >= 0) ylikely {
	    *rpp = op->va ;
	    rs = op->i ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_getvec) */

int vecint_mkvec(vecint *op,VECINT_TYPE *va) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = vecint_magic(op,va)) >= 0) ylikely {
	    for (int i = 0 ; i < op->i ; i += 1) {
		const VECINT_TYPE	v = op->va[i] ;
		if (v != VECINT_MIN) {
		    va[c++] = op->va[i] ;
		}
	    } /* end for */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecint_mkvec) */

int vecint_curbegin(vecint *op,vecint_cur *curp) noex {
	int		rs ;
	if ((rs = vecint_magic(op,curp)) >= 0) ylikely {
	    curp->i = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_curend) */

int vecint_curend(vecint *op,vecint_cur *curp) noex {
	int		rs ;
	if ((rs = vecint_magic(op,curp)) >= 0) ylikely {
	    curp->i = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (vecint_curend) */

int vecint_curenum(vecint *op,vecint_cur *curp,VECINT_TYPE *rp) noex {
	int		rs ;
	int		v = 0 ;
	if ((rs = vecint_magic(op,curp)) >= 0) ylikely {
	    int		i = curp->i ;
	    if ((i >= 0) && (i < op->i)) {
	        v = intconv(op->va[i]) ;
	        curp->i = (i+1) ;
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if (magic) */
	if (rp) *rp = (rs >= 0) ? v : VECINT_MIN ;
	return rs ;
}
/* end subroutine (vecint_curenum) */

int vecint_audit(vecint *op) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = vecint_magic(op)) >= 0) ylikely {
	    volatile VECINT_TYPE	dummy{} ;
	    int			i = 0 ; /* <- used afterwards */
	    for (i = 0 ; i < op->i ; i += 1) {
	        dummy += op->va[i] ;
	    }
	    (void) dummy ;
	    c = op->c ;
	    rs = (i == c) ? SR_OK : SR_BADFMT ;
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecint_audit) */


/* private subroutines */

consteval int mkoptmask() noex {
	int		m = 0 ;
	m |= vecintm.reuse ;
	m |= vecintm.compact ;
	m |= vecintm.swap ;
	m |= vecintm.stationary ;
	m |= vecintm.conserve ;
	m |= vecintm.sorted ;
	m |= vecintm.ordered ;
	return m ;
}
/* end subroutine (mkoptmask) */

local int vecint_setopts(vecint *op,int vo) noex {
	constexpr int	optmask = mkoptmask() ;
	int		rs = SR_INVALID ;
	if ((vo & (~ optmask)) == 0) ylikely {
	    rs = SR_OK ;
	    op->fl = {} ;
	    if (vo & vecintm.reuse)		op->fl.oreuse = true ;
	    if (vo & vecintm.swap)		op->fl.oswap = true ;
	    if (vo & vecintm.stationary)	op->fl.ostationary = true ;
	    if (vo & vecintm.compact)		op->fl.ocompact = true ;
	    if (vo & vecintm.sorted)		op->fl.osorted = true ;
	    if (vo & vecintm.ordered)		op->fl.oordered = true ;
	    if (vo & vecintm.conserve)		op->fl.oconserve = true ;
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (vecint_setopts) */

local int vecint_addval(vecint *op,VECINT_TYPE v) noex {
	int		rs = SR_OK ;
	int		i = 0 ; /* ¥ GCC false complaint */
	bool		f_done = false ;
	bool		f ;
	/* can we fit this new entry within the existing extent? */
	f = (op->fl.oreuse || op->fl.oconserve) && (! op->fl.oordered) ;
	if (f && (op->c < op->i)) {
	    i = op->fi ;
	    while ((i < op->i) && (op->va[i] != VECINT_MIN)) {
	        i += 1 ;
	    }
	    if (i < op->i) {
	        op->va[i] = v ;
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
	    if (rs >= 0) ylikely {
	        i = op->i ;
	        op->va[(op->i)++] = v ;
	        op->va[op->i] = VECINT_MIN ;
	    }
	} /* end if */
	if (rs >= 0) ylikely {
	    op->c += 1 ;		/* increment list count */
	    op->fl.issorted = false ;
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecint_addval) */

local int vecint_extend(vecint *op,int amount) noex {
	int		rs = SR_OK ;
	if (amount > 0) ylikely {
	    cint		esz = szof(VECINT_TYPE) ;
	    int			nn ;
	    int			sz ;
	    VECINT_TYPE		*nva{} ;
	    if (op->va == nullptr) {
	        nn = max(amount,VECINT_DEFENTS) ;
	        sz = ((nn + 1) * esz) ;
	        rs = libmem.mall(sz,&nva) ;
	    } else {
	        nn = max((op->n + amount),(op->n * 2)) ;
	        sz = ((nn + 1) * esz) ;
	        rs = libmem.rall(op->va,sz,&nva) ;
	    } /* end if */
	    if (rs >= 0) {
	        op->va = nva ;
	        op->n = nn ;
		op->va[op->i] = VECINT_MIN ;
	    }
	} /* end if (needed) */
	return rs ;
}
/* end subroutine (vecint_extend) */

local int vecint_insertval(vecint *op,int ii,VECINT_TYPE val) noex {
	if (ii < op->i) ylikely {
	    int		i ; /* used-multiple */
	    /* find */
	    for (i = (ii + 1) ; i < op->i ; i += 1) {
		if (op->va[i] == VECINT_MIN) break ;
	    }
	    /* management */
	    if (i == op->i) {
	        op->i += 1 ;
	        op->va[op->i] = VECINT_MIN ;
	    }
	    /* move-up */
	    for (int j = i ; j > ii ; j -= 1) {
		op->va[j] = op->va[j-1] ;
	    }
	} else if (ii == op->i) {
	    op->i += 1 ;
	    op->va[op->i] = VECINT_MIN ;
	} /* end if */
	op->va[ii] = val ;
	op->c += 1 ;
	op->fl.issorted = false ;
	return ii ;
}
/* end subroutine (vecint_insertval) */

local int vecint_extrange(vecint *op,int n) noex {
	if (n > op->i) {
	    cint	nsz = ((n-op->i) * szof(VECINT_TYPE)) ;
	    memclear((op->va+op->i),nsz) ;
	    op->i = n ;
	    op->va[op->i] = VECINT_MIN ;
	}
	return SR_OK ;
}
/* end subroutine (vecint_extrange) */

local int deftypecmp(const VECINT_TYPE *l1p,const VECINT_TYPE *l2p) noex {
	return intsat(*l1p - *l2p) ;
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

int vecint::addlist(const VECINT_TYPE *ap,int al) noex {
	return vecint_addlist(this,ap,al) ;
}

int vecint::adduniq(VECINT_TYPE v) noex {
	return vecint_adduniq(this,v) ;
}

int vecint::insert(int idx,VECINT_TYPE v) noex {
	return vecint_insert(this,idx,v) ;
}

int vecint::assign(int idx,VECINT_TYPE v) noex {
	return vecint_insert(this,idx,v) ;
}

int vecint::del(int idx) noex {
    	int		rs ;
	if (idx >= 0) {
	    rs = vecint_del(this,idx) ;
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

int vecint::getval(int idx,VECINT_TYPE *rp) noex {
	return vecint_getval(this,idx,rp) ;
}

int vecint::getvec(VECINT_TYPE **rpp) noex {
	return vecint_getvec(this,rpp) ;
}

int vecint::mkvec(VECINT_TYPE *rva) noex {
	return vecint_mkvec(this,rva) ;
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
} /* end method (vecint::dtor) */

int vecint_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
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
} /* end method (vecint_co::operator) */

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
} /* end method (vecint_iter::increment) */



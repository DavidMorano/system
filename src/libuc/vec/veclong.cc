/* veclong SUPPORT */
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
	veclong

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

#include	"veclong.h"

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
local inline int veclong_magic(veclong *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magic == VECLONG_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (veclong_magic) */

local int	veclong_addval(veclong *op,VECLONG_TYPE) noex ;
local int	veclong_extend(veclong *,int) noex ;
local int	veclong_setopts(veclong *,int) noex ;
local int	veclong_insertval(veclong *,int,VECLONG_TYPE) noex ;
local int	veclong_extrange(veclong *,int) noex ;

local int	deftypecmp(const VECLONG_TYPE *,const VECLONG_TYPE *) noex ;


/* local variables */


/* exported variables */

constexpr veclongms	veclongm ;


/* exported subroutines */

int veclong_start(veclong *op,int vn,int vo) noex {
    	VECLONG		*hop = op ;
	int		rs = SR_FAULT ;
	if (vn < 0) vn = VECLONG_DEFENTS ;
	if (op) ylikely {
	    memclear(hop) ;
	    if ((rs = veclong_setopts(op,vo)) >= 0) ylikely {
	        op->n = vn ;
	        if (vn > 0) {
	            cint	sz = (vn + 1) * szof(VECLONG_TYPE) ;
		    if (void *vp ; (rs = libmem.mall(sz,&vp)) >= 0) ylikely {
		        op->va = (VECLONG_TYPE *) vp ;
	    	        op->va[0] = VECLONG_MIN ;
		    }
	        } /* end if (wanted pre-allocation) */
	        if (rs >= 0) {
		    op->magic = VECLONG_MAGIC ;
	        }
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? vn : rs ;
}
/* end subroutine (veclong_start) */

int veclong_finish(veclong *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = veclong_magic(op)) >= 0) ylikely {
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
/* end subroutine (veclong_finish) */

int veclong_add(veclong *op,VECLONG_TYPE v) noex {
	int		rs ;
	if ((rs = veclong_magic(op)) >= 0) ylikely {
	    rs = veclong_addval(op,v) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (veclong_add) */

extern int veclong_addlist(veclong *op,const VECLONG_TYPE *lp,int ll) noex {
	int		rs ;
	if ((rs = veclong_magic(op,lp)) >= 0) ylikely {
	    for (int i = 0 ; (rs >= 0) && (i < ll) ; i += 1) {
	        rs = veclong_addval(op,lp[i]) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (veclong_addlist) */

int veclong_adduniq(veclong *op,VECLONG_TYPE v) noex {
	int		rs ;
	int		i = 0 ;
	if ((rs = veclong_magic(op)) >= 0) ylikely {
	    for (i = 0 ; i < op->i ; i += 1) {
	        if (op->va[i] == v) break ;
	    } /* end for */
	    if (i >= op->i) {
	        rs = veclong_addval(op,v) ;
		i = rs ;
	    } else {
	        i = INT_MAX ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (veclong_adduniq) */

int veclong_insert(veclong *op,int ii,VECLONG_TYPE val) noex {
	int		rs ;
	if ((rs = veclong_magic(op)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (ii >= 0) ylikely {
		rs = SR_OK ;
	        if ((ii+1) > op->n) {
	            rs = veclong_extend(op,((ii+1)-op->n)) ;
	        }
	        if (rs >= 0) {
		    if ((rs = veclong_extrange(op,(ii+1))) >= 0) {
	                rs = veclong_insertval(op,ii,val) ;
		    }
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (veclong_insert) */

int veclong_assign(veclong *op,int ii,VECLONG_TYPE val) noex {
	int		rs ;
	if ((rs = veclong_magic(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (ii >= 0) {
		rs = SR_OK ;
	        if ((ii + 1) > op->n) {
		    cint n = ((ii + 1) - op->n) ;
	            rs = veclong_extend(op,n) ;
	        }
	        if (rs >= 0) ylikely {
		    if ((rs = veclong_extrange(op,(ii + 1))) >= 0) {
	                op->va[ii] = val ;
	            }
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (veclong_assign) */

int veclong_resize(veclong *op,int n) noex {
	int		rs ;
	if ((rs = veclong_magic(op)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (n >= 0) ylikely {
		rs = SR_OK ;
	        if (n != op->i) {
	            if (n > op->n) {
	                rs = veclong_extend(op,(n-op->n)) ;
	            }
	            if (rs >= 0) {
		        if ((rs = veclong_extrange(op,n)) >= 0) {
		            if (n < op->i) {
			        op->i = n ;
		            }
		            op->c = n ;
		            op->va[op->i] = VECLONG_MIN ;
		        }
	            } /* end if (ok) */
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (veclong_resize) */

int veclong_getval(veclong *op,int i,VECLONG_TYPE *rp) noex {
	int		rs ;
	if ((rs = veclong_magic(op)) >= 0) ylikely {
	     if ((i < 0) || (i >= op->i)) {
		rs = SR_NOTFOUND ;
	     }
	     if (rp) {
	         *rp = (rs >= 0) ? op->va[i] : VECLONG_MIN ;
	     }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (veclong_getval) */

int veclong_del(veclong *op,int i) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = veclong_magic(op)) >= 0) ylikely {
	    rs = SR_NOTFOUND ;
	    if ((i >= 0) && (i < op->i)) {
	        bool	f_fi = false ;
		rs = SR_OK ;
		/* delete the entry */
	        op->c -= 1 ;			/* decrement list count */
		/* apply the appropriate deletion based on management policy */
	        if (op->fl.ostationary) {
	            (op->va)[i] = VECLONG_MIN ;
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
	                op->va[op->i] = VECLONG_MIN ;
	            } else {
	                op->va[i] = VECLONG_MIN ;
	                if (i == (op->i - 1)) {
	                    op->i -= 1 ;
		        }
	                f_fi = true ;
	            } /* end if */
	        } else {
		    cbool f = (op->fl.oswap || op->fl.ocompact) ;
	            if (f && (i < (op->i - 1))) {
	                op->va[i] = op->va[op->i - 1] ;
	                op->va[--op->i] = VECLONG_MIN ;
	                op->fl.issorted = false ;
	            } else {
	                op->va[i] = VECLONG_MIN ;
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
/* end subroutine (veclong_del) */

int veclong_delall(veclong *op) noex {
	int		rs ;
	if ((rs = veclong_magic(op)) >= 0) ylikely {
	    op->i = 0 ;
	    op->c = 0 ;
	    op->fi = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (veclong_delall) */

int veclong_count(veclong *op) noex {
	int		rs ;
	if ((rs = veclong_magic(op)) >= 0) ylikely {
	    rs = op->c ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (veclong_count) */

int veclong_extent(veclong *op) noex {
	int		rs ;
	if ((rs = veclong_magic(op)) >= 0) ylikely {
	    rs = op->i ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (veclong_extent) */

int veclong_sort(veclong *op) noex {
	int		rs ;
	if ((rs = veclong_magic(op)) >= 0) ylikely {
	    if (! op->fl.issorted) {
	        op->fl.issorted = true ;
	        if (op->c > 1) {
		    csize	alen = size_t(op->i) ;
		    csize	esize = sizeof(VECLONG_TYPE) ;
		    qsort_f	qcf = qsort_f(deftypecmp) ;
	            qsort(op->va,alen,esize,qcf) ;
	        }
	    }
	    rs = op->c ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (veclong_sort) */

int veclong_setsorted(veclong *op) noex {
	int		rs ;
	if ((rs = veclong_magic(op)) >= 0) ylikely {
	    op->fl.issorted = true ;
	    rs = op->c ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (veclong_setsorted) */

int veclong_find(veclong *op,VECLONG_TYPE v) noex {
	int		rs ;
	int		i = 0 ; /* ¥ GCC false complaint */
	if ((rs = veclong_magic(op)) >= 0) ylikely {
	    if (op->fl.issorted) {
		csize	alen = size_t(op->i) ;
	        csize	esize = sizeof(VECLONG_TYPE) ;
	        qsort_f	qcf = qsort_f(deftypecmp) ;
	        VECLONG_TYPE	*rpp ;
	        rpp = (VECLONG_TYPE *) bsearch(&v,op->va,alen,esize,qcf) ;
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
/* end subroutine (veclong_find) */

int veclong_match(veclong *op,VECLONG_TYPE v) noex {
	int		rs ;
	if ((rs = veclong_find(op,v)) >= 0) {
	    rs = true ;
	} else if (rs == SR_NOTFOUND) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (veclong_match) */

int veclong_getvec(veclong *op,VECLONG_TYPE **rpp) noex {
	int		rs ;
	if ((rs = veclong_magic(op,rpp)) >= 0) ylikely {
	    *rpp = op->va ;
	    rs = op->i ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (veclong_getvec) */

int veclong_mkvec(veclong *op,VECLONG_TYPE *va) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = veclong_magic(op,va)) >= 0) ylikely {
	    for (int i = 0 ; i < op->i ; i += 1) {
		const VECLONG_TYPE	v = op->va[i] ;
		if (v != VECLONG_MIN) {
		    va[c++] = op->va[i] ;
		}
	    } /* end for */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (veclong_mkvec) */

int veclong_curbegin(veclong *op,veclong_cur *curp) noex {
	int		rs ;
	if ((rs = veclong_magic(op,curp)) >= 0) ylikely {
	    curp->i = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (veclong_curend) */

int veclong_curend(veclong *op,veclong_cur *curp) noex {
	int		rs ;
	if ((rs = veclong_magic(op,curp)) >= 0) ylikely {
	    curp->i = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (veclong_curend) */

int veclong_curenum(veclong *op,veclong_cur *curp,VECLONG_TYPE *rp) noex {
	int		rs ;
	int		v = 0 ;
	if ((rs = veclong_magic(op,curp)) >= 0) ylikely {
	    int		i = curp->i ;
	    if ((i >= 0) && (i < op->i)) {
	        v = intconv(op->va[i]) ;
	        curp->i = (i+1) ;
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if (magic) */
	if (rp) *rp = (rs >= 0) ? v : VECLONG_MIN ;
	return rs ;
}
/* end subroutine (veclong_curenum) */

int veclong_audit(veclong *op) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = veclong_magic(op)) >= 0) ylikely {
	    volatile VECLONG_TYPE	dummy{} ;
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
/* end subroutine (veclong_audit) */


/* private subroutines */

consteval int mkoptmask() noex {
	int		m = 0 ;
	m |= veclongm.reuse ;
	m |= veclongm.compact ;
	m |= veclongm.swap ;
	m |= veclongm.stationary ;
	m |= veclongm.conserve ;
	m |= veclongm.sorted ;
	m |= veclongm.ordered ;
	return m ;
}
/* end subroutine (mkoptmask) */

local int veclong_setopts(veclong *op,int vo) noex {
	constexpr int	optmask = mkoptmask() ;
	int		rs = SR_INVALID ;
	if ((vo & (~ optmask)) == 0) ylikely {
	    rs = SR_OK ;
	    op->fl = {} ;
	    if (vo & veclongm.reuse)		op->fl.oreuse = true ;
	    if (vo & veclongm.swap)		op->fl.oswap = true ;
	    if (vo & veclongm.stationary)	op->fl.ostationary = true ;
	    if (vo & veclongm.compact)		op->fl.ocompact = true ;
	    if (vo & veclongm.sorted)		op->fl.osorted = true ;
	    if (vo & veclongm.ordered)		op->fl.oordered = true ;
	    if (vo & veclongm.conserve)		op->fl.oconserve = true ;
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (veclong_setopts) */

local int veclong_addval(veclong *op,VECLONG_TYPE v) noex {
	int		rs = SR_OK ;
	int		i = 0 ; /* ¥ GCC false complaint */
	bool		f_done = false ;
	bool		f ;
	/* can we fit this new entry within the existing extent? */
	f = (op->fl.oreuse || op->fl.oconserve) && (! op->fl.oordered) ;
	if (f && (op->c < op->i)) {
	    i = op->fi ;
	    while ((i < op->i) && (op->va[i] != VECLONG_MIN)) {
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
	        rs = veclong_extend(op,1) ;
	    }
	    /* link into the list structure */
	    if (rs >= 0) ylikely {
	        i = op->i ;
	        op->va[(op->i)++] = v ;
	        op->va[op->i] = VECLONG_MIN ;
	    }
	} /* end if */
	if (rs >= 0) ylikely {
	    op->c += 1 ;		/* increment list count */
	    op->fl.issorted = false ;
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (veclong_addval) */

local int veclong_extend(veclong *op,int amount) noex {
	int		rs = SR_OK ;
	if (amount > 0) ylikely {
	    cint		esz = szof(VECLONG_TYPE) ;
	    int			nn ;
	    int			sz ;
	    VECLONG_TYPE		*nva{} ;
	    if (op->va == nullptr) {
	        nn = max(amount,VECLONG_DEFENTS) ;
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
		op->va[op->i] = VECLONG_MIN ;
	    }
	} /* end if (needed) */
	return rs ;
}
/* end subroutine (veclong_extend) */

local int veclong_insertval(veclong *op,int ii,VECLONG_TYPE val) noex {
	if (ii < op->i) ylikely {
	    int		i ; /* used-multiple */
	    /* find */
	    for (i = (ii + 1) ; i < op->i ; i += 1) {
		if (op->va[i] == VECLONG_MIN) break ;
	    }
	    /* management */
	    if (i == op->i) {
	        op->i += 1 ;
	        op->va[op->i] = VECLONG_MIN ;
	    }
	    /* move-up */
	    for (int j = i ; j > ii ; j -= 1) {
		op->va[j] = op->va[j-1] ;
	    }
	} else if (ii == op->i) {
	    op->i += 1 ;
	    op->va[op->i] = VECLONG_MIN ;
	} /* end if */
	op->va[ii] = val ;
	op->c += 1 ;
	op->fl.issorted = false ;
	return ii ;
}
/* end subroutine (veclong_insertval) */

local int veclong_extrange(veclong *op,int n) noex {
	if (n > op->i) {
	    cint	nsz = ((n-op->i) * szof(VECLONG_TYPE)) ;
	    memclear((op->va+op->i),nsz) ;
	    op->i = n ;
	    op->va[op->i] = VECLONG_MIN ;
	}
	return SR_OK ;
}
/* end subroutine (veclong_extrange) */

local int deftypecmp(const VECLONG_TYPE *l1p,const VECLONG_TYPE *l2p) noex {
	return intsat(*l1p - *l2p) ;
}
/* end subroutine (deftypecmp) */

int veclong_st::operator () (int vn,int vo) noex {
    	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case 0:
    	        rs = veclong_start(op,vn,vo) ;
		break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}

int veclong::add(VECLONG_TYPE v) noex {
	return veclong_add(this,v) ;
}

int veclong::addlist(const VECLONG_TYPE *ap,int al) noex {
	return veclong_addlist(this,ap,al) ;
}

int veclong::adduniq(VECLONG_TYPE v) noex {
	return veclong_adduniq(this,v) ;
}

int veclong::insert(int idx,VECLONG_TYPE v) noex {
	return veclong_insert(this,idx,v) ;
}

int veclong::assign(int idx,VECLONG_TYPE v) noex {
	return veclong_insert(this,idx,v) ;
}

int veclong::del(int idx) noex {
    	int		rs ;
	if (idx >= 0) {
	    rs = veclong_del(this,idx) ;
	} else {
	    rs = veclong_delall(this) ;
	}
	return rs ;
}

int veclong::find(VECLONG_TYPE v) noex {
	return veclong_find(this,v) ;
}

int veclong::match(VECLONG_TYPE v) noex {
	return veclong_match(this,v) ;
}

int veclong::getval(int idx,VECLONG_TYPE *rp) noex {
	return veclong_getval(this,idx,rp) ;
}

int veclong::getvec(VECLONG_TYPE **rpp) noex {
	return veclong_getvec(this,rpp) ;
}

int veclong::mkvec(VECLONG_TYPE *rva) noex {
	return veclong_mkvec(this,rva) ;
}

int veclong::curbegin(veclong_cur *curp) noex {
	return veclong_curbegin(this,curp) ;
}

int veclong::curend(veclong_cur *curp) noex {
	return veclong_curend(this,curp) ;
}

int veclong::curenum(veclong_cur *curp,VECLONG_TYPE *rp) noex {
	return veclong_curenum(this,curp,rp) ;
}

void veclong::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("veclong",rs,"fini-finish") ;
	}
} /* end method (veclong::dtor) */

int veclong_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case veclongmem_count:
	        rs = veclong_count(op) ;
	        break ;
	    case veclongmem_extent:
	        rs = veclong_extent(op) ;
	        break ;
	    case veclongmem_delall:
	        rs = veclong_delall(op) ;
	        break ;
	    case veclongmem_sort:
	        rs = veclong_sort(op) ;
	        break ;
	    case veclongmem_setsorted:
	        rs = veclong_setsorted(op) ;
	        break ;
	    case veclongmem_resize:
	        rs = veclong_resize(op,a) ;
	        break ;
	    case veclongmem_audit:
	        rs = veclong_audit(op) ;
	        break ;
	    case veclongmem_finish:
	        rs = veclong_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (veclong_co::operator) */

bool veclong_iter::operator == (const veclong_iter &oit) noex {
	return (va == oit.va) && (i == oit.i) && (ii == oit.ii) ;
}

bool veclong_iter::operator != (const veclong_iter &oit) noex {
	bool		f = false ;
	f = f || (va != oit.va) ;
	f = f || (ii != oit.ii) ;
	if (!f) {
	    f = (i < oit.i) ;
	}
	return f ;
}
/* end method (veclong_iter::operator) */

veclong_iter veclong_iter::operator + (int n) const noex {
	veclong_iter	rit(va,i,i) ;
	rit.i = ((rit.i + n) >= 0) ? (rit.i + n) : 0 ;
	return rit ;
}

veclong_iter veclong_iter::operator += (int n) noex {
	veclong_iter	rit(va,i,i) ;
	i = ((i + n) >= 0) ? (i + n) : 0 ;
	rit.i = i ;
	return rit ;
}

veclong_iter veclong_iter::operator ++ () noex { /* pre */
	increment() ;
	return (*this) ;
}

veclong_iter veclong_iter::operator ++ (int) noex { /* post */
	veclong_iter	pre(*this) ;
	increment() ;
	return pre ;
}

void veclong_iter::increment(int n) noex {
	if ((i + n) < 0) n = -i ;
	if (n != 0) {
	    i += n ;
	    while ((i < ii) && (va[i] == -1)) {
	        i += 1 ;
	    }
	}
} /* end method (veclong_iter::increment) */



/* veclong SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* vector long-integer operations */
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
	These routines are used when the caller wants to store a
	COPY of the passed element data into a vector.  These
	routines will copy and store the copied data in the list.
	The advantage is that the caller does not have to keep the
	orginal data around in order for the list data to be accessed
	later.  Element data (unlike string data) can contain nullptr
	characters-bytes.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<localmisc.h>

#include	"veclong.h"

import libutil ;

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


/* external subroutines */

extern "C" {
    int		veclong_add(veclong *,VECLONG_TYPE) noex ;
}


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int veclong_magic(veclong *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == VECLONG_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (veclong_magic) */

static int	veclong_addval(veclong *op,VECLONG_TYPE) noex ;
static int	veclong_extend(veclong *,int) noex ;
static int	veclong_setopts(veclong *,int) noex ;
static int	veclong_insertval(veclong *,int,VECLONG_TYPE) noex ;
static int	veclong_extrange(veclong *,int) noex ;

static int	deflongcmp(const VECLONG_TYPE *,const VECLONG_TYPE *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int veclong_start(veclong *op,int n,int opts) noex {
	int		rs = SR_FAULT ;
	if (n < 0) n = VECLONG_DEFENTS ;
	if (op) {
	    memclear(op) ;
	    if ((rs = veclong_setopts(op,opts)) >= 0) {
	        op->n = n ;
	        op->magic = VECLONG_MAGIC ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (veclong_start) */

int veclong_finish(veclong *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = veclong_magic(op)) >= 0) {
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
/* end subroutine (veclong_finish) */

int veclong_add(veclong *op,VECLONG_TYPE v) noex {
	int		rs ;
	if ((rs = veclong_magic(op)) >= 0) {
	    rs = veclong_addval(op,v) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (veclong_add) */

int veclong_addlist(veclong *op,const VECLONG_TYPE *lp,int ll) noex {
	int		rs ;
	if ((rs = veclong_magic(op)) >= 0) {
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
	if ((rs = veclong_magic(op)) >= 0) {
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
	if ((rs = veclong_magic(op)) >= 0) {
	    if (ii >= 0) {
	        if ((ii+1) > op->n) {
	            rs = veclong_extend(op,((ii+1)-op->n)) ;
	        }
	        if (rs >= 0) {
		    if ((rs = veclong_extrange(op,(ii+1))) >= 0) {
	                rs = veclong_insertval(op,ii,val) ;
		    }
	        }
	    } else {
	        rs = SR_INVALID ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (veclong_insert) */

int veclong_assign(veclong *op,int ii,VECLONG_TYPE val)  noex {
	int		rs ;
	if ((rs = veclong_magic(op)) >= 0) {
	    if (ii >= 0) {
	        if ((ii+1) > op->n) {
	            rs = veclong_extend(op,((ii+1)-op->n)) ;
	        }
	        if (rs >= 0) {
		    if ((rs = veclong_extrange(op,(ii+1))) >= 0) {
	                op->va[ii] = val ;
		    }
	        }
	    } else {
	        rs = SR_INVALID ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (veclong_assign) */

int veclong_resize(veclong *op,int n) noex {
	int		rs ;
	if ((rs = veclong_magic(op)) >= 0) {
	    if (n >= 0) {
	        if (n > op->n) {
	            rs = veclong_extend(op,(n-op->n)) ;
	        }
	        if (rs >= 0) {
		    if ((rs = veclong_extrange(op,n)) >= 0) {
		        op->c = n ;
		        op->va[n] = LONG_MIN ;
		    }
	        }
	    } else {
	        rs = SR_INVALID ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (veclong_resize) */

int veclong_getval(veclong *op,int i,VECLONG_TYPE *rp) noex {
	int		rs ;
	if ((rs = veclong_magic(op)) >= 0) {
	    if ((i < 0) || (i >= op->i)) {
	        rs = SR_NOTFOUND ;
	    }
	    if (rp) {
	        *rp = (rs >= 0) ? op->va[i] : long(-1) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (veclong_getval) */

int veclong_mkvec(veclong *op,VECLONG_TYPE *va) noex {
    	typedef VECLONG_TYPE	val_t ;
	int		rs ;
	int		c = 0 ;
	if ((rs = veclong_magic(op,va)) >= 0) {
	    if (va) {
	        cint	n = op->i ;
	        for (int i = 0 ; i < n ; i += 1) {
		    val_t	tv = op->va[i] ;
		    if (tv != val_t(INT_MIN)) {
		        va[c++] = op->va[i] ;
		    }
	        } /* end for */
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (veclong_mkvec) */

int veclong_curbegin(veclong *op,veclong_cur *curp) noex {
	int		rs ;
	if ((rs = veclong_magic(op,curp)) >= 0) {
	    curp->i = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (veclong_curend) */

int veclong_curend(veclong *op,veclong_cur *curp) noex {
	int		rs ;
	if ((rs = veclong_magic(op)) >= 0) {
	    curp->i = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (veclong_end) */

int veclong_curenum(veclong *op,veclong_cur *curp,VECLONG_TYPE *rp) noex {
    	typedef VECLONG_TYPE	val_t ;
	int		rs ;
	int		i = 0 ;
	if ((rs = veclong_magic(op,curp,rp)) >= 0) {
	    val_t	tv{} ;
	    i = curp->i ;
	    if ((i >= 0) && (i < op->i)) {
	        tv = (op->va)[i] ;
	        curp->i = (i+1) ;
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	    if (rp) *rp = (rs >= 0) ? tv : val_t(INT_MIN) ;
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (veclong_curenum) */

int veclong_del(veclong *op,int i) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = veclong_magic(op)) >= 0) {
	    rs = SR_NOTFOUND ;
	    if ((i >= 0) && (i < op->i)) {
	        bool	f_fi = false ;
		rs = SR_OK ;
		/* delete the entry */
	        op->c -= 1 ;		/* decrement list count */
                /* apply the appropriate deletion based on management policy */
	        if (op->fl.ostationary) {
	            op->va[i] = 0 ;
	            if (i == (op->i - 1)) {
	                op->i -= 1 ;
	            }
	            f_fi = true ;
	        } else if (op->fl.issorted || op->fl.oordered) {
	             if (op->fl.ocompact) {
		         int	j ;
	                 op->i -= 1 ;
	                 for (j = i ; j < op->i ; j += 1) {
	                     op->va[j] = op->va[j + 1] ;
		         }
	                 op->va[op->i] = 0 ;
	             } else {
	                 op->va[i] = 0 ;
	                 if (i == (op->i - 1)) {
	                     op->i -= 1 ;
		         }
	                 f_fi = true ;
	             } /* end if */
	         } else {
	             if ((op->fl.oswap || op->fl.ocompact) && (i < (op->i - 1))) {
	                 op->va[i] = op->va[op->i - 1] ;
	                 op->va[--op->i] = 0 ;
	                 op->fl.issorted = false ;
	             } else {
	                 op->va[i] = 0 ;
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
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (veclong_del) */

int veclong_count(veclong *op) noex {
	int		rs ;
	if ((rs = veclong_magic(op)) >= 0) {
	    rs = op->c ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (veclong_count) */

int veclong_sort(veclong *op) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = veclong_magic(op)) >= 0) {
	    if (! op->fl.issorted) {
	        op->fl.issorted = true ;
	        if (op->c > 1) {
		    cint	esz = szof(VECLONG_TYPE) ;
	            sort_vcmp	scmp = sort_vcmp(deflongcmp) ;
	            qsort(op->va,op->i,esz,scmp) ;
	        }
	    }
	    c = op->c ;
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (veclong_sort) */

int veclong_setsorted(veclong *op) noex {
	int		rs ;
	if ((rs = veclong_magic(op)) >= 0) {
	    op->fl.issorted = true ;
	    rs = op->c ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (veclong_setsorted) */

int veclong_find(veclong *op,VECLONG_TYPE v) noex {
	int		rs ;
	int		i = 0 ;
	if ((rs = veclong_magic(op)) >= 0) {
	    if (op->fl.issorted) {
	        long		*rpp2 ;
	        cint		esz = szof(VECLONG_TYPE) ;
	        sort_vcmp	scmp = sort_vcmp(deflongcmp) ;
	        rpp2 = (long *) bsearch(&v,op->va,op->i,esz,scmp) ;
	        rs = SR_NOTFOUND ;
	        if (rpp2 != nullptr) {
	            i = intconv(rpp2 - op->va) ;
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
	if ((rs = veclong_magic(op,rpp)) >= 0) {
	    *rpp = op->va ;
	    rs = op->i ; 
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (veclong_getvec) */

int veclong_audit(veclong *op) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = veclong_magic(op)) >= 0) {
	    long	v = 0 ;
	    for (int i = 0 ; i < op->i ; i += 1) {
	        c += 1 ;
	        v |= op->va[i] ;
	    }
	    rs = (c == op->c) ? SR_OK : SR_BADFMT ;
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (veclong_audit) */


/* private subroutines */

static consteval int mkoptmask() noex {
	int		m = 0 ;
	m |= VECLONG_OREUSE ;
	m |= VECLONG_OSWAP ;
	m |= VECLONG_OSTATIONARY ;
	m |= VECLONG_OCOMPACT ;
	m |= VECLONG_OSORTED ;
	m |= VECLONG_OORDERED ;
	m |= VECLONG_OCONSERVE ;
	return m ;
} /* end subroutine (mkoptmask) */

static int veclong_setopts(veclong *op,int vo) noex {
	constexpr int	m = mkoptmask() ;
	int		rs = SR_INVALID ;
	if ((vo & (~ m)) == 0) {
	    rs = SR_OK ;
	    op->fl = {} ;
	    if (vo & VECLONG_OREUSE) op->fl.oreuse = 1 ;
	    if (vo & VECLONG_OSWAP) op->fl.oswap = 1 ;
	    if (vo & VECLONG_OSTATIONARY) op->fl.ostationary = 1 ;
	    if (vo & VECLONG_OCOMPACT) op->fl.ocompact = 1 ;
	    if (vo & VECLONG_OSORTED) op->fl.osorted = 1 ;
	    if (vo & VECLONG_OORDERED) op->fl.oordered = 1 ;
	    if (vo & VECLONG_OCONSERVE) op->fl.oconserve = 1 ;
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (veclong_setopts) */

int veclong_addval(veclong *op,VECLONG_TYPE v) noex {
	int		rs = SR_OK ;
	int		i = 0 ; /* ¥ GCC false complaint */
	bool		f_done = false ;
	bool		f ;
	/* can we fit this new entry within the existing extent? */
	f = (op->fl.oreuse || op->fl.oconserve) && (! op->fl.oordered) ;
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
	        rs = veclong_extend(op,1) ;
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
	    op->fl.issorted = false ;
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (veclong_addval) */

static int veclong_extend(veclong *op,int amount) noex {
	int		rs = SR_OK ;
	if (amount > 0) {
	    cint		esize = szof(VECLONG_TYPE) ;
	    int			nn ;
	    int			sz ;
	    VECLONG_TYPE	*nva ;
	    if (op->va == nullptr) {
	        nn = max(amount,VECLONG_DEFENTS) ;
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
	    }
	}
	return rs ;
}
/* end subroutine (veclong_extend) */

static int veclong_insertval(veclong *op,int ii,VECLONG_TYPE val) noex {
	if (ii < op->i) {
	    int		i ;
	    /* find */
	    for (i = (ii + 1) ; i < op->i ; i += 1) {
		if (op->va[i] == LONG_MIN) break ;
	    }
	    /* management */
	    if (i == op->i) {
	        op->i += 1 ;
	        op->va[op->i] = LONG_MIN ;
	    }
	    /* move-up */
	    for (int j = i ; j > ii ; j -= 1) {
		op->va[j] = op->va[j-1] ;
	    }
	} else if (ii == op->i) {
	    op->i += 1 ;
	    op->va[op->i] = LONG_MIN ;
	} /* end if */
	op->va[ii] = val ;
	op->c += 1 ;
	op->fl.issorted = false ;
	return ii ;
}
/* end subroutine (veclong_insertval) */

static int veclong_extrange(veclong *op,int n) noex {
	if (n > op->i) {
	    cint	nsz = ((n - op->i) * szof(VECLONG_TYPE)) ;
	    memset((op->va + op->i),0,nsz) ;
	    op->i = n ;
	    op->va[op->i] = LONG_MIN ;
	}
	return SR_OK ;
}
/* end subroutine (veclong_extrange) */

static int deflongcmp(const VECLONG_TYPE *l1p,const VECLONG_TYPE  *l2p) noex {
	return intsat(*l1p - *l2p) ;
}
/* end subroutine (deflongcmp) */



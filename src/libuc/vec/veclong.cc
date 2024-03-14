/* veclong SUPPORT */
/* lang=C++20 */

/* vector long-integer operations */
/* version %I% last-modified %G% */

#define	CF_SAFE		1		/* pointer safety */

/* revision history:

	= 1998-12-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	These routines are used when the caller wants to store a
	COPY of the passed element data into a vector.  These
	routines will copy and store the copied data in the list.
	The advantage is that the caller does not have to keep the
	orginal data around in order for the list data to be accessed
	later.  Element data (unlike string data) can contain nullptr
	characters-bytes.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"veclong.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef int (*qsortcmp_f)(cvoid *,cvoid *) noex ;
}


/* external subroutines */

extern "C" {
    int		veclong_add(veclong *,VECLONG_TYPE) noex ;
}


/* external variables */


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
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (n < 0)
	    n = VECLONG_DEFENTS ;

	memset(op,0,sizeof(veclong)) ;

	if ((rs = veclong_setopts(op,opts)) >= 0) {
	    op->n = n ;
	    op->magic = VECLONG_MAGIC ;
	}

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (veclong_start) */

int veclong_finish(veclong *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != VECLONG_MAGIC) return SR_NOTOPEN ;
#endif

	if (op->va != nullptr) {
	    rs1 = uc_free(op->va) ;
	    if (rs >= 0) rs = rs1 ;
	    op->va = nullptr ;
	}

	op->c = 0 ;
	op->i = 0 ;
	op->n = 0 ;
	op->magic = 0 ;

	return rs ;
}
/* end subroutine (veclong_finish) */

int veclong_add(veclong *op,VECLONG_TYPE v) noex {
	if (op == nullptr) return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != VECLONG_MAGIC) return SR_NOTOPEN ;
#endif

	return veclong_addval(op,v) ;
}
/* end subroutine (veclong_add) */

int veclong_addlist(veclong *op,const VECLONG_TYPE *lp,int ll) noex {
	int		rs = SR_OK ;
	int		i ;

	if (op == nullptr) return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != VECLONG_MAGIC) return SR_NOTOPEN ;
#endif

	for (i = 0 ; (rs >= 0) && (i < ll) ; i += 1) {
	    rs = veclong_addval(op,lp[i]) ;
	}

	return rs ;
}
/* end subroutine (veclong_addlist) */

int veclong_adduniq(veclong *op,VECLONG_TYPE v) noex {
	int		rs = INT_MAX ;
	int		i ;

	if (op == nullptr) return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != VECLONG_MAGIC) return SR_NOTOPEN ;
#endif

/* first, search for this value */

	for (i = 0 ; i < op->i ; i += 1) {
	    if (op->va[i] == v) break ;
	} /* end for */

	if (i >= op->i) {
	    rs = veclong_addval(op,v) ;
	}

	return rs ;
}
/* end subroutine (veclong_adduniq) */

int veclong_insert(veclong *op,int ii,VECLONG_TYPE val) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != VECLONG_MAGIC) return SR_NOTOPEN ;
#endif

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

	return rs ;
}
/* end subroutine (veclong_insert) */

int veclong_assign(veclong *op,int ii,VECLONG_TYPE val)  noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != VECLONG_MAGIC) return SR_NOTOPEN ;
#endif

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

	return rs ;
}
/* end subroutine (veclong_assign) */

int veclong_resize(veclong *op,int n) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != VECLONG_MAGIC) return SR_NOTOPEN ;
#endif

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

	return rs ;
}
/* end subroutine (veclong_resize) */

int veclong_getval(veclong *op,int i,VECLONG_TYPE *rp) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != VECLONG_MAGIC) return SR_NOTOPEN ;
#endif

	if ((i < 0) || (i >= op->i)) {
	    rs = SR_NOTFOUND ;
	}

	if (rp) {
	    *rp = (rs >= 0) ? op->va[i] : long(-1) ;
	}

	return rs ;
}
/* end subroutine (veclong_getval) */

int veclong_mkvec(veclong *op,VECLONG_TYPE *va) noex {
	int		c = 0 ;

	if (op == nullptr) return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != VECLONG_MAGIC) return SR_NOTOPEN ;
#endif

	if (va != nullptr) {
	    cint	n = op->i ;
	    int		i ;
	    int		v ;
	    for (i = 0 ; i < n ; i += 1) {
		v = (op->va)[i] ;
		if (v != INT_MIN) {
		    va[c++] = (op->va)[i] ;
		}
	    } /* end for */
	} /* end if */

	return c ;
}
/* end subroutine (veclong_mkvec) */

int veclong_curbegin(veclong *op,veclong_cur *curp) noex {
	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (op->magic != VECLONG_MAGIC) return SR_NOTOPEN ;
	curp->i = 0 ;
	return SR_OK ;
}
/* end subroutine (veclong_curend) */

int veclong_curend(veclong *op,veclong_cur *curp) noex {
	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (op->magic != VECLONG_MAGIC) return SR_NOTOPEN ;
	curp->i = 0 ;
	return SR_OK ;
}
/* end subroutine (veclong_end) */

int veclong_enum(veclong *op,veclong_cur *curp,VECLONG_TYPE *rp) noex {
	int		rs = SR_OK ;
	int		i ;
	int		v = 0 ;
	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (op->magic != VECLONG_MAGIC) return SR_NOTOPEN ;
	i = curp->i ;
	if ((i >= 0) && (i < op->i)) {
	    v = (op->va)[i] ;
	    curp->i = (i+1) ;
	} else {
	    rs = SR_NOTFOUND ;
	}
	if (rp != nullptr) *rp = (rs >= 0) ? v : INT_MIN ;
	return rs ;
}
/* end subroutine (veclong_enum) */

int veclong_del(veclong *op,int i) noex {
	int		f_fi = false ;

	if (op == nullptr) return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != VECLONG_MAGIC) return SR_NOTOPEN ;
#endif

	if ((i < 0) || (i >= op->i))
	    return SR_NOTFOUND ;

/* delete the entry */

	    op->c -= 1 ;		/* decrement list count */

/* apply the appropriate deletion based on management policy */

	if (op->f.ostationary) {

	    (op->va)[i] = 0 ;
	    if (i == (op->i - 1))
	        op->i -= 1 ;

	    f_fi = true ;

	} else if (op->f.issorted || op->f.oordered) {

	    if (op->f.ocompact) {
		int	j ;

	        op->i -= 1 ;
	        for (j = i ; j < op->i ; j += 1)
	            (op->va)[j] = (op->va)[j + 1] ;

	        (op->va)[op->i] = 0 ;

	    } else {

	        (op->va)[i] = 0 ;
	        if (i == (op->i - 1))
	            op->i -= 1 ;

	        f_fi = true ;

	    } /* end if */

	} else {

	    if ((op->f.oswap || op->f.ocompact) && (i < (op->i - 1))) {

	        (op->va)[i] = (op->va)[op->i - 1] ;
	        (op->va)[--op->i] = 0 ;
	        op->f.issorted = false ;

	    } else {

	        (op->va)[i] = 0 ;
	        if (i == (op->i - 1))
	            op->i -= 1 ;

	        f_fi = true ;

	    } /* end if */

	} /* end if */

	if (f_fi && (i < op->fi)) {
	    op->fi = i ;
	}

	return op->c ;
}
/* end subroutine (veclong_del) */

int veclong_count(veclong *op) noex {
	if (op == nullptr) return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != VECLONG_MAGIC) return SR_NOTOPEN ;
#endif

	return op->c ;
}
/* end subroutine (veclong_count) */

int veclong_sort(veclong *op) noex {
	if (op == nullptr) return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != VECLONG_MAGIC) return SR_NOTOPEN ;
#endif

	if (! op->f.issorted) {
	    op->f.issorted = true ;
	    if (op->c > 1) {
		cint		esz = sizeof(VECLONG_TYPE) ;
	        sort_vcmp	scmp = sort_vcmp(deflongcmp) ;
	        qsort(op->va,op->i,esz,scmp) ;
	    }
	}

	return op->c ;
}
/* end subroutine (veclong_sort) */

int veclong_setsorted(veclong *op) noex {
	if (op == nullptr) return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != VECLONG_MAGIC) return SR_NOTOPEN ;
#endif

	op->f.issorted = true ;
	return op->c ;
}
/* end subroutine (veclong_setsorted) */

int veclong_find(veclong *op,VECLONG_TYPE v) noex {
	int		rs = SR_OK ;
	int		i = 0 ;

	if (op == nullptr) return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != VECLONG_MAGIC) return SR_NOTOPEN ;
#endif

	if (op->f.issorted) {
	    long	*rpp2 ;
	    cint	esz = sizeof(VECLONG_TYPE) ;
	    sort_vcmp	scmp = sort_vcmp(deflongcmp) ;
	    rpp2 = (long *) bsearch(&v,op->va,op->i,esz,scmp) ;

	    rs = SR_NOTFOUND ;
	    if (rpp2 != nullptr) {
	        i = rpp2 - op->va ;
	        rs = SR_OK ;
	    }

	} else {

	    for (i = 0 ; i < op->i ; i += 1) {
	        if (op->va[i] == v) break ;
	    } /* end for */
	    rs = (i < op->i) ? SR_OK : SR_NOTFOUND ;

	} /* end if */

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

int veclong_getvec(VECLONG *op,VECLONG_TYPE **rpp) noex {
	if (op == nullptr) return SR_FAULT ;
	if (rpp == nullptr) return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != VECLONG_MAGIC) return SR_NOTOPEN ;
#endif

	*rpp = op->va ;
	return op->i ;
}
/* end subroutine (veclong_getvec) */

int veclong_audit(VECLONG *op) noex {
	long		v = 0 ;
	int		rs = SR_OK ;
	int		i ;
	int		c = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != VECLONG_MAGIC) return SR_NOTOPEN ;

	for (i = 0 ; i < op->i ; i += 1) {
	    c += 1 ;
	    v |= op->va[i] ;
	}

	rs = (c == op->c) ? SR_OK : SR_BADFMT ;
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (veclong_audit) */


/* private subroutines */

static int veclong_setopts(VECLONG *op,int options) noex {
	int		rs = SR_OK ;
	op->f = {} ;
	if (options & VECLONG_OREUSE) op->f.oreuse = 1 ;
	if (options & VECLONG_OSWAP) op->f.oswap = 1 ;
	if (options & VECLONG_OSTATIONARY) op->f.ostationary = 1 ;
	if (options & VECLONG_OCOMPACT) op->f.ocompact = 1 ;
	if (options & VECLONG_OSORTED) op->f.osorted = 1 ;
	if (options & VECLONG_OORDERED) op->f.oordered = 1 ;
	if (options & VECLONG_OCONSERVE) op->f.oconserve = 1 ;
	return rs ;
}
/* end subroutine (veclong_setopts) */

int veclong_addval(veclong *op,VECLONG_TYPE v) noex {
	int		rs = SR_OK ;
	int		i = 0 ; /* ¥ GCC false complaint */
	int		f_done = false ;
	int		f ;

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
	    op->f.issorted = false ;
	}

	return (rs >= 0) ? i : rs ;
}
/* end subroutine (veclong_addval) */

static int veclong_extend(veclong *op,int amount) noex {
	int		rs = SR_OK ;
	if (amount > 0) {
	    cint		esize = sizeof(VECLONG_TYPE) ;
	    int			nn, size ;
	    VECLONG_TYPE	*va ;
	    if (op->va == nullptr) {
	        nn = MAX(amount,VECLONG_DEFENTS) ;
	        size = ((nn + 1) * esize) ;
	        rs = uc_malloc(size,&va) ;
	    } else {
	        nn = MAX((op->n+amount),(op->n*2)) ;
	        size = ((nn + 1) * esize) ;
	        rs = uc_realloc(op->va,size,&va) ;
	    } /* end if */
	    if (rs >= 0) {
	        op->va = va ;
	        op->n = nn ;
	    }
	}

	return rs ;
}
/* end subroutine (veclong_extend) */

static int veclong_insertval(veclong *op,int ii,VECLONG_TYPE val) noex {
	if (ii < op->i) {
	    int		i, j ;

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

	    for (j = i ; j > ii ; j -= 1) {
		op->va[j] = op->va[j-1] ;
	    }

	} else if (ii == op->i) {
	    op->i += 1 ;
	    op->va[op->i] = LONG_MIN ;
	} /* end if */

	op->va[ii] = val ;
	op->c += 1 ;
	op->f.issorted = false ;

	return ii ;
}
/* end subroutine (veclong_insertval) */

static int veclong_extrange(veclong *op,int n) noex {
	if (n > op->i) {
		cint nsz = ((n-op->i)*sizeof(VECLONG_TYPE)) ;
		memset((op->va+op->i),0,nsz) ;
	    op->i = n ;
	    op->va[op->i] = LONG_MIN ;
	}
	return SR_OK ;
}
/* end subroutine (veclong_extrange) */

static int deflongcmp(const VECLONG_TYPE *l1p,const VECLONG_TYPE  *l2p) noex {
	return (*l1p - *l2p) ;
}
/* end subroutine (deflongcmp) */



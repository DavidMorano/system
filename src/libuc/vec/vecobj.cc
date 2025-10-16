/* vecobj SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* vector object list operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	vecobj

	Description:
	These routines are used when the caller wants to store a
	COPY of the passed object data into a vector.  These routines
	will copy and store the copied data in the list.  The
	advantage is that the caller does not have to keep the
	orginal data around in order for the list data to be accessed
	later.  Element data (unlike string data) can contain NUL
	characters-bytes.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* <- for |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- |memcmp(3c)| */
#include	<new>
#include	<algorithm>
#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<lookaside.h>
#include	<localmisc.h>

#include	"vecobj.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef vecobj_cur	cur ;
typedef vecobj_vcf	c_f ;

extern "C" {
    typedef int (*qsort_f)(cvoid *,cvoid *) noex ;
    typedef int (*vg_f)(vecobj *,int,void **) noex ;
}


/* forward references */

static int	vecobj_ctor(vecobj *) noex ;
static int	vecobj_dtor(vecobj *) noex ;
static int	vecobj_setopts(vecobj *,int) noex ;
static int	vecobj_extend(vecobj *) noex ;
static int	vecobj_iget(vecobj *,int,void **) noex ;
static int	vecobj_sorted(vecobj *,vecobj_vcf) noex ;

static int	mkoptmask() noex ;


/* local subroutines */


/* local variables */

cint			defents = VECOBJ_DEFENTS ;

static cint		optmask = mkoptmask() ;


/* exported variables */

int vecobjms::reuse		= (1 << vecobjo_reuse) ;
int vecobjms::compact		= (1 << vecobjo_compact) ;
int vecobjms::swap		= (1 << vecobjo_swap) ;
int vecobjms::stationary	= (1 << vecobjo_stationary) ;
int vecobjms::conserve		= (1 << vecobjo_conserve) ;
int vecobjms::sorted		= (1 << vecobjo_sorted) ;
int vecobjms::ordered		= (1 << vecobjo_ordered) ;

const vecobjms		vecobjm ;


/* exported subroutines */

int vecobj_start(vecobj *op,int osize,int n,int opts) noex {
	int		rs ;
	if ((rs = vecobj_ctor(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (osize > 0) {
	        if (n <= 0) n = defents ;
	        op->va = nullptr ;
	        op->esz = osize ;
	        if ((rs = vecobj_setopts(op,opts)) >= 0) {
	            cint	size = (n + 1) * szof(char **) ;
	            op->i = 0 ;
	            op->c = 0 ;
	            op->fi = 0 ;
	            if (void *vp ; (rs = libmem.mall(size,&vp)) >= 0) {
	                op->va = voidpp(vp) ;
	                op->va[0] = nullptr ;
	                op->n = n ;
	                rs = lookaside_start(op->lap,osize,n) ;
	                if (rs < 0) {
	                    libmem.free(vp) ;
			    op->va = nullptr ;
			}
	            } /* end if */
	        } /* end if (options) */
	    } /* end if (valid) */
	    if (rs < 0) {
		vecobj_dtor(op) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (vecobj_start) */

int vecobj_finish(vecobj *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = SR_OK ;
		{
		    rs1 = lookaside_finish(op->lap) ;
		    if (rs >= 0) rs = rs1 ;
		}
		{
		    rs1 = libmem.free(op->va) ;
		    if (rs >= 0) rs = rs1 ;
		    op->va = nullptr ;
		}
		{
		    rs1 = vecobj_dtor(op) ;
		    if (rs >= 0) rs = rs1 ;
		}
		op->c = 0 ;
		op->i = 0 ;
		op->n = 0 ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecobj_finish) */

int vecobj_add(vecobj *op,cvoid *s) noex {
	int		rs ;
	if (void *ep ; (rs = vecobj_addnew(op,&ep)) >= 0) {
	    memcpy(ep,s,op->esz) ;
	}
	return rs ;
}
/* end subroutine (vecobj_add) */

int vecobj_adduniq(vecobj *op,cvoid *ep) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && ep) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
	        const caddr_t	*vepp ; 
	        cint		esize = op->esz ;
	        rs = INT_MAX ;
	        for (i = 0 ; i < op->i ; i += 1) {
	            vepp = (caddr_t *) op->va ;
	            vepp += i ;
	            if (memcmp(*vepp,ep,esize) == 0) break ;
	        } /* end for */
	        if (i >= op->i) {
	            rs = vecobj_add(op,ep) ;
	        }
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecobj_adduniq) */

int vecobj_addnew(vecobj *op,void **epp) noex {
	int		rs  SR_FAULT ;
	int		i = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
	        void	*sp = nullptr ;		/* storage pointer */
	        if ((rs = lookaside_get(op->lap,&sp)) >= 0) {
	            bool	f = true ;
	            bool	f_done = false ;
	            f = f && (op->fl.oreuse || op->fl.oconserve) ;
		    f = f && (! op->fl.oordered) ;
	            if (f && (op->c < op->i)) {
	                i = op->fi ;
	                while ((i < op->i) && (op->va[i] != nullptr)) {
	                    i += 1 ;
		        }
	                if (i < op->i) {
	                    op->va[i] = sp ;
	                    op->fi = i + 1 ;
	                    f_done = true ;
	                } else {
	                    op->fi = i ;
		        }
	            } /* end if (possible reuse strategy) */
	            if (! f_done) {
	                if ((op->i + 1) > op->n) {
	                    rs = vecobj_extend(op) ;
		        }
	                if (rs >= 0) {
	                    i = op->i ;
	                    op->va[(op->i)++] = sp ;
	                    op->va[op->i] = nullptr ;
	                }
	            } /* end if (added elsewhere) */
	            if (rs >= 0) {
	                op->c += 1 ;		/* increment list count */
	                op->fl.issorted = false ;
	            } else {
	                lookaside_release(op->lap,sp) ;
	            }
	        } /* end if (entry allocated) */
	        if (epp) *epp = (rs >= 0) ? sp : nullptr ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecobj_addnew) */

int vecobj_inorder(vecobj *op,cvoid *cep,vecobj_vcf vcf,int cn) noex {
	int		rs = SR_FAULT ;
	int		ei = 0 ;
	if (op && cep && vcf) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = SR_OK ;
	        if (op->fl.osorted) {
	            if (! op->fl.issorted) {
	                op->fl.issorted = true ;
	                if (op->c > 1) {
	                    cint	esize = szof(void *) ;
		            qsort_f	scf = qsort_f(vcf) ;
	                    qsort(op->va,op->i,esize,scf) ;
	                }
	            } /* end if (sorting) */
	            if (cn != 0) {
	                int	cr = 1 ;
	                int	n = (cn > 0) ? MIN(cn,op->i) : op->i ;
	                int	i = 0 ;
	                if ((cn > 0) && (n > 0)) {
	                    for (i = (n - 1) ; i >= 0 ; i -= 1) {
	                        if (op->va[i]) break ;
	                    }
	                    if (i >= 0) {
			        cvoid	**vap = (cvoid **) (op->va+i) ;
	                        cr = (*vcf)(&cep,vap) ;
	                        if (cr >= 0) i = n ;
	                    } else {
	                        i = 0 ;
		            }
	                } /* end if (conditional insertion) */
	                if ((cn < 0) || (cr < 0)) {
	                    for (i = 0 ; i < n ; i += 1) {
	                        if (op->va[i]) {
			            cvoid	**vap = (cvoid **) (op->va+i) ;
	                            cr = (*vcf)(&cep,vap) ;
	                            if (cr < 0) break ;
			        }
	                    } /* end for */
	                } /* end if (finding insert point) */
	                if (i < n) {
	                    void	*tp ;
	                    ei = i ;
	                    if ((rs = vecobj_add(op,cep)) >= 0) {
	                        tp = op->va[rs] ;
	                        for (int j = (op->i - 1) ; j > i ; j -= 1) {
	                            op->va[j] = op->va[j - 1] ;
	                        }
	                        op->va[i] = tp ;
	                    } /* end if */
	                } else if ((cn < 0) || (i < cn)) {
	                    rs = vecobj_add(op,cep) ;
	                    ei = rs ;
	                } else {
	                    rs = SR_NOTFOUND ;
		        }
	            } else {
	                rs = SR_NOTFOUND ;
	            }
	        } else {
	            rs = vecobj_add(op,cep) ;
	            ei = rs ;
	        }
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (vecobj_inorder) */

int vecobj_store(vecobj *op,cvoid *s,void **rpp) noex {
	int		rs ;
	int		i = 0 ;
	if ((rs = vecobj_add(op,s)) >= 0) {
	    i = rs ;
	    if (rpp) {
	        rs = vecobj_get(op,i,rpp) ;
	    }
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecobj_store) */

int vecobj_get(vecobj *op,int i,void **rpp) noex {
	int		rs = SR_FAULT ;
	if (op && rpp) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = SR_OK ;
	        if ((i >= 0) && (i < op->i)) {
	            *rpp = op->va[i] ;
	        } else {
	            *rpp = nullptr ;
	            rs = SR_NOTFOUND ;
	        }
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecobj_get) */

int vecobj_del(vecobj *op,int i) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = SR_NOTFOUND ;
	        if ((i >= 0) && (i < op->i)) {
		    rs = SR_OK ;
	            if (op->va[i]) {
	                op->c -= 1 ;		/* decrement list count */
	                rs = lookaside_release(op->lap,op->va[i]) ;
	                op->va[i] = nullptr ;
	            }
	            if (rs >= 0) {
		    	bool	f_fi = false ;
	                if (op->fl.ostationary) {
	                    op->va[i] = nullptr ;
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
	                        op->va[op->i] = nullptr ;
	                    } else {
	                        op->va[i] = nullptr ;
	                        if (i == (op->i - 1)) {
	                            op->i -= 1 ;
		                }
	                        f_fi = true ;
	                    } /* end if */
	                } else {
			    bool	f = (op->fl.oswap || op->fl.ocompact) ;
			    f = f && (i < (op->i - 1)) ;
	                    if (f) {
	                        op->va[i] = op->va[op->i - 1] ;
	                        op->va[--op->i] = nullptr ;
	                        op->fl.issorted = false ;
	                    } else {
	                        op->va[i] = nullptr ;
	                        if (i == (op->i - 1)) {
	                            op->i -= 1 ;
		                }
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
	            } /* end if (ok) */
		    c = op->c ;
		} /* given index in-range) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecobj_del) */

int vecobj_delall(vecobj *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = SR_OK ;
	        for (int i = 0 ; i < op->i ; i += 1) {
	            if (op->va[i]) {
	                rs1 = lookaside_release(op->lap,op->va[i]) ;
	                if (rs >= 0) rs = rs1 ;
	            }
	        } /* end for */
	        op->va[0] = nullptr ;
	        op->i = 0 ;
	        op->fi = 0 ;
	        op->c = 0 ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecobj_delall) */

int vecobj_count(vecobj *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = op->c ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecobj_count) */

int vecobj_sort(vecobj *op,vecobj_vcf vcf) noex {
	int		rs = SR_FAULT ;
	if (op && vcf) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = op->c ;
	        if (! op->fl.issorted) {
	            op->fl.issorted = true ;
	            if (op->c > 1) {
	                cint		esize = szof(void *) ;
		        qsort_f		scf = qsort_f(vcf) ;
	                qsort(op->va,op->i,esize,scf) ;
	            }
	        }
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecobj_sort) */

int vecobj_setsorted(vecobj *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = op->c ;
		op->fl.issorted = true ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecobj_setsorted) */

int vecobj_curbegin(vecobj *op,vecobj_cur *curp) noex {
	int		rs = SR_FAULT ;
	if (op && curp) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = SR_OK ;
	        curp->i = -1 ;
	        curp->c = -1 ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecobj_curbegin) */

int vecobj_curend(vecobj *op,vecobj_cur *curp) noex {
	int		rs = SR_FAULT ;
	if (op && curp) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = SR_OK ;
	        curp->i = -1 ;
	        curp->c = -1 ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecobj_curend) */

namespace {
    struct sub_fetch {
	vecobj		*op ;
	vecobj_vcf	vcf ;
	cvoid		*ep ;
	void		**rpp ;
	sub_fetch(vecobj *p,cvoid *e,c_f c,void **r) noex : op(p), vcf(c) {
	    ep = e ;
	    rpp = r ;
	} ;
	int first(cur *) noex ;
	int next(cur *) noex ;
    } ; /* end struct (sub_fetch) */
} /* end namespace */

int vecobj_curfetch(vecobj *op,cvoid *ep,cur *curp,c_f vcf,void **rpp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && ep && vcf) {
	    rs = SR_NOTFOUND ;
	    if (op->va) {
	 	sub_fetch	so(op,ep,vcf,rpp) ;
	        vecobj_cur	tmpcur{} ;
		rs = SR_OK ;
	        if (curp == nullptr) {
	            curp = &tmpcur ;
	            curp->i = -1 ;
	        }
	        if (curp->i < 0) {
		    rs = so.first(curp) ;
		    i = rs ;
	        } else {
		    rs = so.next(curp) ;
		    i = rs ;
	        } /* end if (first or subsequent fetch) */
		if (rs < 0) {
	            rs = SR_NOTFOUND ;
	            if (rpp) *rpp = nullptr ;
	        } /* end if (error) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecobj_curfetch) */

int vecobj_search(vecobj *op,cvoid *ep,vecobj_vcf vcf,void **rpp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && ep && rpp) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		if ((rs = vecobj_sorted(op,vcf)) > 0) {
		    csize	nsize = size_t(op->i) ;
	            csize	esize = sizeof(void *) ;
	            qsort_f	scf = qsort_f(vcf) ;
	            void	**sepp ;
	            sepp = (void **) bsearch(&ep,op->va,nsize,esize,scf) ;
	            rs = SR_NOTFOUND ;
	            if (sepp) {
	                rs = SR_OK ;
	                i = intconv(charpp(sepp) - charpp(op->va)) ;
	            }
	        } else {
	            for (i = 0 ; i < op->i ; i += 1) {
	                if (op->va[i]) {
			    cvoid	**cvpp = cvoidpp(op->va + i) ;
	                    if (vcf(&ep,cvpp) == 0) break ;
		        }
	            } /* end for */
	            rs = (i < op->i) ? SR_OK : SR_NOTFOUND ;
	        } /* end if (sorted or not) */
	        if (rpp) {
	            *rpp = (rs >= 0) ? op->va[i] : nullptr ;
	        }
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecobj_search) */

int vecobj_find(vecobj *op,cvoid *cep) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
	        for (i = 0 ; i < op->i ; i += 1) {
		    cvoid	*ep = op->va[i] ;
	            if (ep) {
	                if (memcmp(cep,ep,op->esz) == 0) break ;
	            }
	        } /* end for */
	        rs = (i < op->i) ? SR_OK : SR_NOTFOUND ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecobj_find) */

int vecobj_getvec(vecobj *op,void ***rppp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && rppp) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = SR_OK ;
		*rppp = op->va ;
		i = op->i ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecobj_getvec) */

int vecobj_audit(vecobj *op) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
                for (int i = 0 ; i < op->i ; i += 1) {
	            if (op->va[i]) {
	                int	*ip = (int *) op->va[i] ;
	                rs |= *ip ;		/* access might SEGFAULT */
	                c += 1 ;
	            }
	        } /* end for */
	        rs = (c == op->c) ? SR_OK : SR_BADFMT ;
	        if (rs >= 0) {
	            rs = lookaside_audit(op->lap) ;
	        }
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecobj_audit) */


/* private subroutines */

static int vecobj_ctor(vecobj *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    const nullptr_t	np{} ;
	    rs = SR_NOMEM ;
	    op->va = nullptr ;
	    op->c = 0 ;
	    op->i = 0 ;
	    op->n = 0 ;
	    op->fi = 0 ;
	    op->esz = 0 ;
	    if ((op->lap = new(nothrow) lookaside) != np) {
		rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecobj_ctor) */

static int vecobj_dtor(vecobj *op) noex {
	int		rs = SR_OK ;
	if (op->lap) {
	    delete op->lap ;
	    op->lap = nullptr ;
	}
	return rs ;
}
/* end subroutine (vecobj_dtor) */

static int mkoptmask() noex {
	int		m = 0 ;
	m |= vecobjm.reuse ;
	m |= vecobjm.compact ;
	m |= vecobjm.swap ;
	m |= vecobjm.stationary ;
	m |= vecobjm.conserve ;
	m |= vecobjm.sorted ;
	m |= vecobjm.ordered ;
	return m ;
}
/* end subroutine (mkoptmask) */

static int vecobj_setopts(vecobj *op,int vo) noex {
	int		rs = SR_INVALID ;
	if ((vo & (~ optmask)) == 0) {
	    rs = SR_OK ;
	    op->fl = {} ;
	    if (vo & vecobjm.reuse)		op->fl.oreuse = true ;
	    if (vo & vecobjm.swap)		op->fl.oswap = true ;
	    if (vo & vecobjm.stationary)	op->fl.ostationary = true ;
	    if (vo & vecobjm.compact)		op->fl.ocompact = true ;
	    if (vo & vecobjm.sorted)		op->fl.osorted = true ;
	    if (vo & vecobjm.ordered)		op->fl.oordered = true ;
	    if (vo & vecobjm.conserve)		op->fl.oconserve = true ;
	} /* end if (valid options) */
	return rs ;
}
/* end subroutine (vecobj_setopts) */

static int vecobj_extend(vecobj *op) noex {
	int		rs = SR_OK ;
	if ((op->i + 1) > op->n) {
	    int		nn ;
	    int		sz ;
	    void	*nva{} ;
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
/* end subroutine (vecobj_extend) */

static int vecobj_iget(vecobj *op,int i,void **rpp) noex {
	int		rs = SR_NOTFOUND ;
	if ((i >= 0) && (i < op->i)) {
	    *rpp = op->va[i] ;
	    rs = i ;
	} else {
	    *rpp = nullptr ;
	}
	return rs ;
}
/* end subroutine (vecobj_iget) */

static int vecobj_sorted(vecobj *op,vecobj_vcf vcf) noex {
	int		rs = SR_OK ;
	int		fsorted ;
	if (op->fl.osorted && (! op->fl.issorted)) {
	    op->fl.issorted = true ;
	    if (op->c > 1) {
	        cint	esize = szof(void *) ;
		qsort_f	scf = qsort_f(vcf) ;
		qsort(op->va,op->i,esize,scf) ;
	    }
	} /* end if (sorting) */
	fsorted = op->fl.issorted ;
	return (rs >= 0) ? fsorted : rs ;
}
/* end subroutine (vecobj_sorted) */

int sub_fetch::first(cur *curp) noex {
	int		rs ;
	int		i = 0 ;
	void		*rep{} ;
	if ((rs = vecobj_search(op,ep,vcf,&rep)) >= 0) {
	    i = rs ;
	    if (op->fl.osorted) {
		vg_f	v = vecobj_iget ;
		int	j = (rs - 1) ; /* used-afterwards */
		void	*first = rep ;
	        while ((j >= 0) && ((rs = v(op,j,&rep)) >= 0)) {
		    cvoid	*cep = cvoidp(rep) ;
		    if (vcf(&ep,&cep) != 0) break ;
		    first = rep ;
		    j -= 1 ;
		} /* end while */
		curp->i = (j + 1) ;
		rep = first ;
	    } /* end if (sorted) */
	} /* end if */
	if (rs >= 0) {
	    curp->i = i ;
	    if (rpp) *rpp = rep ;
	}
	return (rs >= 0) ? i : rs ;
}
/* end method (sub_fetch::first) */

int sub_fetch::next(cur *curp) noex {
	int		rs ;
	int		i = (curp->i + 1) ; 
	void		*rep{} ;
	while ((rs = vecobj_iget(op,i,&rep)) >= 0) {
	    if (rep) {
		cvoid	*cep = cvoidp(rep) ;
		if (vcf(&ep,&cep) == 0) break ;
	    }
	    i += 1 ;
	} /* end while */
	if (rs >= 0) {
	    curp->i = i ;
	    if (rpp) *rpp = rep ;
	}
	return (rs >= 0) ? i : rs ;
}
/* end method (sub_fetch::next) */

int vecobj::start(int osz,int vn,int vo) noex {
	return vecobj_start(this,osz,vn,vo) ;
}

int vecobj::add(cvoid *ep) noex {
	return vecobj_add(this,ep) ;
}

int vecobj::adduniq(cvoid *ep) noex {
	return vecobj_adduniq(this,ep) ;
}

int vecobj::store(cvoid *ep,void **rpp) noex {
	return vecobj_store(this,ep,rpp) ;
}

int vecobj::find(cvoid *ep) noex {
	return vecobj_find(this,ep) ;
}

int vecobj::get(int ei,void **rpp) noex {
	return vecobj_get(this,ei,rpp) ;
}

int vecobj::getvec(void ***rppp) noex {
	return vecobj_getvec(this,rppp) ;
}

int vecobj::del(int ai) noex {
	if (ai < 0) ai = 0 ;
	return vecobj_del(this,ai) ;
}

int vecobj::search(cvoid *ep,vecobj_vcf vcf,void **rpp) noex {
	return vecobj_search(this,ep,vcf,rpp) ;
}

int vecobj::sort(vecobj_vcf vcf) noex {
	return vecobj_sort(this,vcf) ;
}

int vecobj::curbegin(vecobj_cur *curp) noex {
	return vecobj_curbegin(this,curp) ;
}

int vecobj::curend(vecobj_cur *curp) noex {
	return vecobj_curend(this,curp) ;
}

int vecobj::curfetch(cvoid *ep,cur *curp,c_f vcf,void **rpp) noex {
	return vecobj_curfetch(this,ep,curp,vcf,rpp) ;
}

void vecobj::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("vecobj",rs,"fini-finish") ;
	}
}

vecobj::operator int () noex {
    	int		rs = SR_NOTOPEN ;
	if (n > 0) {
	    rs = c ;
	}
	return rs ;
}

vecobj_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case vecobjmem_count:
	        rs = vecobj_count(op) ;
	        break ;
	    case vecobjmem_delall:
	        rs = vecobj_delall(op) ;
	        break ;
	    case vecobjmem_audit:
	        rs = vecobj_audit(op) ;
	        break ;
	    case vecobjmem_finish:
	        rs = vecobj_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (vecobj_co::operator) */



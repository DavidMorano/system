/* vecobj SUPPORT */
/* lang=C++20 */

/* vector object list operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	These routines are used when the caller wants to store a
	COPY of the passed object data into a vector.  These routines
	will copy and store the copied data in the list.  The
	advantage is that the caller does not have to keep the
	orginal data around in order for the list data to be accessed
	later.  Element data (unlike string data) can contain NUL
	characters-bytes.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<climits>		/* <- for |INT_MAX| */
#include	<cstdlib>
#include	<cstring>
#include	<new>
#include	<algorithm>
#include	<usystem.h>
#include	<lookaside.h>
#include	<localmisc.h>

#include	"vecobj.h"


/* local defines */


/* local namespaces */

using std::min ;			/* actor */
using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef vecobj_vcmp	c_f ;

extern "C" {
    typedef int (*qsort_f)(cvoid *,cvoid *) noex ;
    typedef int (*rawc_f)(void **,void **) noex ;
    typedef int (*vg_f)(vecobj *,int,void **) noex ;
}


/* forward references */

static int	vecobj_ctor(vecobj *) noex ;
static int	vecobj_dtor(vecobj *) noex ;
static int	vecobj_setopts(vecobj *,int) noex ;
static int	vecobj_extend(vecobj *) noex ;
static int	vecobj_iget(vecobj *,int,void **) noex ;


/* local subroutines */

consteval int mkoptmask() noex {
	int		m = 0 ;
	m |= VECOBJ_OREUSE ;
	m |= VECOBJ_OCOMPACT ;
	m |= VECOBJ_OSWAP ;
	m |= VECOBJ_OSTATIONARY ;
	m |= VECOBJ_OCONSERVE ;
	m |= VECOBJ_OSORTED ;
	m |= VECOBJ_OORDERED ;
	return m ;
}
/* end subroutine (mkoptmask) */


/* local variables */

constexpr int		optmask = mkoptmask() ;


/* exported subroutines */

int vecobj_start(vecobj *op,int osize,int n,int opts) noex {
	int		rs ;
	if ((rs = vecobj_ctor(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (osize > 0) {
	        if (n <= 0) n = VECOBJ_DEFENTS ;
	        op->va = nullptr ;
	        op->esize = osize ;
	        if ((rs = vecobj_setopts(op,opts)) >= 0) {
	            cint	size = (n + 1) * sizeof(char **) ;
	            void	*vp{} ;
	            op->i = 0 ;
	            op->c = 0 ;
	            op->fi = 0 ;
	            if ((rs = uc_libmalloc(size,&vp)) >= 0) {
	                op->va = (void **) vp ;
	                op->va[0] = nullptr ;
	                op->n = n ;
	                rs = lookaside_start(op->lap,osize,n) ;
	                if (rs < 0) {
	                    uc_libfree(vp) ;
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
		    rs1 = uc_libfree(op->va) ;
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
	void		*ep ;
	if ((rs = vecobj_addnew(op,&ep)) >= 0) {
	    memcpy(ep,s,op->esize) ;
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
	        cint		esize = op->esize ;
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
	            bool	f = true ;;
	            bool	f_done = false ;
	            f = f && (op->f.oreuse || op->f.oconserve) ;
		    f = f && (! op->f.oordered) ;
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
	                op->f.issorted = false ;
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

int vecobj_inorder(vecobj *op,cvoid *cep,vecobj_vcmp vcf,int cn) noex {
	int		rs = SR_FAULT ;
	int		ei = 0 ;
	if (op && cep && vcf) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = SR_OK ;
	        if (op->f.osorted) {
	            if (! op->f.issorted) {
	                op->f.issorted = true ;
	                if (op->c > 1) {
	                    cint	esize = sizeof(void *) ;
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

int vecobj_get(vecobj *op,int i,void *rp) noex {
	void		**rpp = (void **) rp ;
	int		rs = SR_FAULT ;
	if (op && rp) {
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

int vecobj_store(vecobj *op,cvoid *s,void *rp) noex {
	int		rs ;
	int		i = 0 ;
	if ((rs = vecobj_add(op,s)) >= 0) {
	    i = rs ;
	    if (rp) {
	        rs = vecobj_get(op,i,rp) ;
	    }
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecobj_store) */

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
	                if (op->f.ostationary) {
	                    op->va[i] = nullptr ;
	                    if (i == (op->i - 1)) {
	                        op->i -= 1 ;
		            }
	                    f_fi = true ;
	                } else if (op->f.issorted || op->f.oordered) {
	                    if (op->f.ocompact) {
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
			    bool	f = (op->f.oswap || op->f.ocompact) ;
			    f = f && (i < (op->i - 1)) ;
	                    if (f) {
	                        op->va[i] = op->va[op->i - 1] ;
	                        op->va[--op->i] = nullptr ;
	                        op->f.issorted = false ;
	                    } else {
	                        op->va[i] = nullptr ;
	                        if (i == (op->i - 1)) {
	                            op->i -= 1 ;
		                }
	                        f_fi = true ;
	                    } /* end if */
	                } /* end if */
	                if (op->f.oconserve) {
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

int vecobj_sort(vecobj *op,vecobj_vcmp vcf) noex {
	int		rs = SR_FAULT ;
	if (op && vcf) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = op->c ;
	        if (! op->f.issorted) {
	            op->f.issorted = true ;
	            if (op->c > 1) {
	                cint		esize = sizeof(void *) ;
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
		op->f.issorted = true ;
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

int vecobj_fetch(vecobj *op,void *ep,vecobj_cur *curp,c_f vcf,void *rp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && ep && vcf) {
	    rs = SR_NOTFOUND ;
	    if (op->va) {
	        vecobj_cur	cur ;
	        int		j ;
	        rawc_f		raw = rawc_f(vcf) ;
	        void		**rpp = (void **) rp ;
	        void		*ep2 ;
		rs = SR_OK ;
	        if (curp == nullptr) {
	            curp = &cur ;
	            curp->i = -1 ;
	        }
	        if (curp->i < 0) {
	            curp->c = 0 ;
	            if (rpp) rpp = &ep2 ;
	            if ((rs = vecobj_search(op,ep,vcf,rpp)) >= 0) {
		        cint	js = (rs-1) ;
	                if (op->f.osorted) {
			    vg_f	v = vecobj_iget ;
	                    void	*last = *rpp ;
	                    for (j = js ;
	                        (j >= 0) && ((rs = v(op,j,rpp)) >= 0) ; 
	                        j -= 1) {
	                        if (raw(&ep,rpp) != 0) break ;
	                        last = *rpp ;
	                    } /* end for */
	                    curp->i = j + 1 ;
	                    rs = curp->i ;
	                    *rpp = last ;
	                } else {
	                    curp->i = rs ;
		        }
	            } /* end if */
	            if (rs >= 0) {
	                curp->i = rs ;
	                curp->c = 1 ;
	            }
	        } else {
	            if (op->f.osorted) {
	                if (! op->f.issorted) {
	                    int		si ;
	                    op->f.issorted = true ;
	                    if (op->c > 1) {
	                        cint		esize = sizeof(void *) ;
		                qsort_f		scf = qsort_f(vcf) ;
	                        qsort(op->va,op->i,esize,scf) ;
	                    }
	                    if ((si = vecobj_search(op,ep,vcf,rpp)) >= 0) {
	                        i = si ;
	                        if (si > 0) {
	                            void	*last = *rpp ;
	                            for (j = (si - 1) ;
	                                (j >= 0) && 
	                                ((rs = vecobj_iget(op,j,rpp)) >= 0) ; 
	                                j -= 1) {
	                                if (raw(&ep,rpp) != 0) break ;
	                                last = *rpp ;
	                            } /* end for */
	                            i = j + 1 ;
	                            *rpp = last ;
	                        } /* end if */
	                        i += (curp->c - 1) ;
	                        curp->i = i ;
	                    } else {
	                        curp->i = op->i ;
		            }
	                } /* end if (it was out-of-order) */
	                i = curp->i + 1 ;
	                if ((rs = vecobj_iget(op,i,&ep2)) >= 0) {
	                    if (raw(&ep,&ep2) != 0) {
	                        rs = SR_NOTFOUND ;
		            }
	                } /* end if */
	            } else {
	                for (i = (curp->i + 1) ; 
	                    (rs = vecobj_iget(op,i,&ep2)) >= 0 ; i += 1) {
	                    if (ep2) {
	                        if (raw(&ep,&ep2) == 0) break ;
		            }
	                } /* end for */
	            } /* end if (sorted policy or not) */
	            if (rs >= 0) {
	                rs = curp->i = i ;
	                curp->c += 1 ;
	                if (rpp) *rpp = ep2 ;
	            } /* end if (ok) */
	        } /* end if (first or subsequent fetch) */
	        if (rs < 0) {
	            rs = SR_NOTFOUND ;
	            if (rpp) *rpp = nullptr ;
	        } /* end if (error) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecobj_fetch) */

int vecobj_search(vecobj *op,void *ep,vecobj_vcmp vcf,void *vrp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && ep && vcf) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
	        rawc_f	raw = rawc_f(vcf) ;
	        void	**rpp = (void **) vrp ;
		rs = SR_OK ;
	        if (op->f.osorted && (! op->f.issorted)) {
	            op->f.issorted = true ;
	            if (op->c > 1) {
	                cint	esize = sizeof(void *) ;
		        qsort_f	scf = qsort_f(vcf) ;
	                qsort(op->va,op->i,esize,scf) ;
	            }
	        } /* end if (sorting) */
	        if (op->f.issorted) {
	            cint	esize = sizeof(void *) ;
	            qsort_f	scf = qsort_f(vcf) ;
	            void	**rpp2 ;
	            rpp2 = (void **) bsearch(&ep,op->va,op->i,esize,scf) ;
	            rs = SR_NOTFOUND ;
	            if (rpp2) {
	                rs = SR_OK ;
	                i = ((char **) rpp2) - ((char **) op->va) ;
	            }
	        } else {
	            for (i = 0 ; i < op->i ; i += 1) {
	                if (op->va[i]) {
	                    if (raw(&ep,(op->va + i)) == 0) break ;
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
	                if (memcmp(cep,ep,op->esize) == 0) break ;
	            }
	        } /* end for */
	        rs = (i < op->i) ? SR_OK : SR_NOTFOUND ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecobj_find) */

int vecobj_getvec(vecobj *op,void *rp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && rp) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		void	**rpp = (void **) rp ;
		rs = SR_OK ;
		*rpp = op->va ;
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
	    nullptr_t	np{} ;
	    rs = SR_NOMEM ;
	    op->va = nullptr ;
	    op->c = 0 ;
	    op->i = 0 ;
	    op->n = 0 ;
	    op->fi = 0 ;
	    op->esize = 0 ;
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

static int vecobj_setopts(vecobj *op,int vo) noex {
	int		rs = SR_INVALID ;
	if ((vo & (~optmask)) == 0) {
	    rs = SR_OK ;
	    op->f = {} ;
	    if (vo & VECOBJ_OREUSE) op->f.oreuse = 1 ;
	    if (vo & VECOBJ_OSWAP) op->f.oswap = 1 ;
	    if (vo & VECOBJ_OSTATIONARY) op->f.ostationary = 1 ;
	    if (vo & VECOBJ_OCOMPACT) op->f.ocompact = 1 ;
	    if (vo & VECOBJ_OSORTED) op->f.osorted = 1 ;
	    if (vo & VECOBJ_OORDERED) op->f.oordered = 1 ;
	    if (vo & VECOBJ_OCONSERVE) op->f.oconserve = 1 ;
	} /* end if (valid options) */
	return rs ;
}
/* end subroutine (vecobj_setopts) */

static int vecobj_extend(vecobj *op) noex {
	int		rs = SR_OK ;
	if ((op->i + 1) > op->n) {
	    int		nn, size ;
	    void	*np ;
	    if (op->va == nullptr) {
	        nn = VECOBJ_DEFENTS ;
	        size = (nn + 1) * sizeof(void **) ;
	        rs = uc_libmalloc(size,&np) ;
	    } else {
	        nn = (op->n + 1) * 2 ;
	        size = (nn + 1) * sizeof(void **) ;
	        rs = uc_librealloc(op->va,size,&np) ;
	        op->va = nullptr ;
	    }
	    if (rs >= 0) {
	        op->va = (void **) np ;
	        op->n = nn ;
		op->va[op->i] = nullptr ;
	    }
	} /* end if (extension required) */
	return rs ;
}
/* end subroutine (vecobj_extend) */

static int vecobj_iget(vecobj *op,int i,void **spp) noex {
	int		rs = SR_NOTFOUND ;
	if ((i >= 0) && (i < op->i)) {
	    *spp = op->va[i] ;
	    rs = i ;
	} else {
	    *spp = nullptr ;
	}
	return rs ;
}
/* end subroutine (vecobj_iget) */



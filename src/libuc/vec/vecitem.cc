/* vecitem SUPPORT */
/* lang=C++20 */

/* vector-item operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

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
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<localmisc.h>

#include	"vecitem.h"


/* local defines */


/* local namespaces */

using std::nullptr_t ;


/* local typedefs */

extern "C" {
    typedef int (*qsort_f)(cvoid *,cvoid *) noex ;
    typedef vecitem_cmpf	cmpf ;
}

typedef vecitem_cur	cur ;
typedef cvoid *		cvoidp ;
typedef void **		voidpp ;
typedef cvoid **	cvoidpp ;


/* local structures */

namespace {
    struct fetchargs {
	cvoid		*cep ;
	cur		*curp ;
	cmpf		cf ;
	void		**rpp ;
	void operator () (cvoid *acep,cur *acurp,cmpf f,void **arpp) noex {
	    cep = acep ;
	    curp = acurp ;
	    cf = f ;
	    rpp = arpp ;
	} ;
   } ; /* end struct (fetchargs) */
}



/* forward references */

int		vecitem_search(vecitem *,cvoid *,cmpf,void *) noex ;

static int	vecitem_setopts(vecitem *,int) noex ;
static int	vecitem_extend(vecitem *) noex ;
static int	vecitem_iget(vecitem *,int,void **) noex ;
static int	vecitem_fetchbeg(vecitem *,fetchargs *) noex ;
static int	vecitem_fetchcont(vecitem *,fetchargs *) noex ;

consteval int mkoptmask() noex {
	int		m = 0 ;
	m |= VECITEM_OREUSE ;
	m |= VECITEM_OCOMPACT ;
	m |= VECITEM_OSWAP ;
	m |= VECITEM_OSTATIONARY ;
	m |= VECITEM_OCONSERVE ;
	m |= VECITEM_OSORTED ;
	m |= VECITEM_OORDERED ;
	return m ;
}
/* end subroutine (mkoptmask) */

consteval cur mkcurdef() noex {
	cur	c{} ;
	c.c = 0 ;
	c.i = -1 ;
	return c ;
} ;


/* local variables */

constexpr cur		curdef = mkcurdef() ;


/* exported subroutines */

int vecitem_start(vecitem *op,int n,int opts) noex {
	int		rs = SR_FAULT ;
	if (n < 0) n = VECITEM_DEFENTS ;
	if (op) {
	    memclear(op) ;	/* <- potentially dangerous if type changes */
	    if ((rs = vecitem_setopts(op,opts)) >= 0) {
	        cint	size = (n + 1) * sizeof(char **) ;
		void	*vp{} ;
	        if ((rs = uc_malloc(size,&vp)) >= 0) {
		    op->va = (void **) vp ;
	            op->va[0] = nullptr ;
	            op->n = n ;
	        }
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (vecitem_start) */

int vecitem_finish(vecitem *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    for (int i = 0 ; i < op->i ; i += 1) {
		void	*ep = op->va[i] ;
	        if (ep) {
	            rs1 = uc_free(ep) ;
	            if (rs >= 0) rs = rs1 ;
		    op->va[i] = nullptr ;
	        }
	    } /* end for */
	    {
	        rs1 = uc_free(op->va) ;
	        if (rs >= 0) rs = rs1 ;
	        op->va = nullptr ;
	    }
	    op->c = 0 ;
	    op->i = 0 ;
	    op->n = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecitem_finish) */

int vecitem_add(vecitem *op,void *ep,int el) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && ep) {
	    char	*bp = nullptr ;
	    bool	f_done = false ;
	    bool	f ;
	    if (el < 0) el = strlen(charp(ep)) ;
	    if ((rs = uc_malloc((el+1),&bp)) >= 0) {
	        memcpy(bp,ep,el) ;
	        bp[el] = '\0' ;
	        f = (op->f.oreuse || op->f.oconserve) && (! op->f.oordered) ;
	        if (f && (op->c < op->i)) {
	            i = op->fi ;
	            while ((i < op->i) && (op->va[i] != nullptr)) {
	                i += 1 ;
		    }
	            if (i < op->i) {
	                op->va[i] = bp ;
	                op->fi = (i + 1) ;
	                f_done = true ;
	            } else {
	                op->fi = i ;
		    }
	        } /* end if (possible reuse strategy) */
	        if (! f_done) {
	            if ((op->i + 1) > op->n) {
	                rs = vecitem_extend(op) ;
	            }
	            if (rs >= 0) {
	                i = op->i ;
	                op->va[(op->i)++] = bp ;
	                op->va[op->i] = nullptr ;
	            }
	        } /* end if (added elsewhere) */
	        if (rs >= 0) {
	            op->c += 1 ; /* increment list count */
	        } else {
	            if (bp != nullptr) {
	                uc_free(bp) ;
	            }
	        }
	        op->f.issorted = false ;
	    } /* end if (m-a) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecitem_add) */

int vecitem_get(vecitem *op,int i,void *vrp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTFOUND ;
	    if ((i >= 0) && (i < op->i)) {
		rs = SR_OK ;
	        if (vrp) {
	            void	**rpp = (void **) vrp ;
	            *rpp = (rs >= 0) ? op->va[i] : nullptr ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecitem_get) */

int vecitem_del(vecitem *op,int i) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_INVALID ;
	    if ((i >= 0) && (i < op->i)) {
		bool	f_fi = false ;
		rs = SR_OK ;
	        if (op->va[i] != nullptr) {
	            op->c -= 1 ;		/* decrement list count */
	            uc_free(op->va[i]) ;
	        } /* end if */
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
	            if ((op->f.oswap || op->f.ocompact) && (i < (op->i - 1))) {
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
	        rs = op->c ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecitem_del) */

int vecitem_count(vecitem *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->c ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecitem_count) */

int vecitem_sort(vecitem *op,cmpf cf) noex {
	int		rs = SR_FAULT ;
	if (op && cf) {
	    rs = op->c ;
	    if (! op->f.issorted) {
	        op->f.issorted = true ;
	        if (op->c > 1) {
		    qsort_f	scf = qsort_f(cf) ;
		    cint	ssz = sizeof(void *) ;
	            qsort(op->va,op->i,ssz,scf) ;
	        }
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecitem_sort) */

int vecitem_curbegin(vecitem *op,cur *curp) noex {
	int		rs = SR_FAULT ;
	if (op && curp) {
	    rs = SR_OK ;
	    *curp = curdef ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (curbegin) */

int vecitem_curend(vecitem *op,cur *curp) noex {
	int		rs = SR_FAULT ;
	if (op && curp) {
	    rs = SR_OK ;
	    *curp = curdef ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (curend) */

int vecitem_fetch(vecitem *op,cvoid *cep,cur *curp,cmpf cf,void *vrp) noex {
	void		**rpp = (void **) vrp ;
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && cep && cf) {
	    fetchargs	a ;
	    cur		acur = curdef ;
	    void	*aep = nullptr ;
	    rs = SR_OK ;
	    if (curp == nullptr) {
	        curp = &acur ;
	    }
	    if (rpp == nullptr) {
		rpp = &aep ;
	    }
	    a(cep,curp,cf,rpp) ;
	    if (curp->i < 0) {
		rs = vecitem_fetchbeg(op,&a) ;
		i = rs ;
	    } else {
		rs = vecitem_fetchcont(op,&a) ;
		i = rs ;
	    } /* end if (first or subsequent fetch) */
	    if (rpp) {
	        if (rs < 0) *rpp = nullptr ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecitem_fetch) */

int vecitem_search(vecitem *op,cvoid *cep,cmpf cf,void *vrp) noex {
	void		**rpp = (void **) vrp ;
	int		rs = SR_OK ;
	int		i = 0 ;
	if (op && cep && cf) {
	    rs = SR_OK ;
	    if (op->f.osorted && (! op->f.issorted)) {
	        op->f.issorted = true ;
	        if (op->c > 1) {
		    qsort_f		scf = qsort_f(cf) ;
		    cint		ssz = sizeof(void *) ;
	            qsort(op->va,op->i,ssz,scf) ;
	        }
	    } /* end if (sorting) */
	    if (op->f.issorted) {
		nullptr_t	np{} ;
	        qsort_f	scf = qsort_f(cf) ;
	        cint	ssz = sizeof(void *) ;
	        void	**spp ;
	        rs = SR_NOTFOUND ;
	        if ((spp = voidpp(bsearch(&cep,op->va,op->i,ssz,scf))) != np) {
	            i = (spp - op->va) ;
	            rs = SR_OK ;
	        }
	    } else {
	        for (i = 0 ; i < op->i ; i += 1) {
		    cvoid	*oep = op->va[i] ;
	            if (oep) {
	                if ((*cf)(&cep,&oep) == 0) break ;
	            }
	        } /* end for */
	        rs = (i < op->i) ? SR_OK : SR_NOTFOUND ;
	    } /* end if (sorted or not) */
	    if (rpp) {
	        *rpp = (rs >= 0) ? op->va[i] : nullptr ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecitem_search) */

int vecitem_find(vecitem *op,cvoid *cep,int len) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && cep) {
	    rs = SR_INVALID ;
	    if (len > 0) {
	        for (i = 0 ; i < op->i ; i += 1) {
		    void	*ep = op->va[i] ;
	            if (ep) {
	                if (memcmp(cep,cep,len) == 0) break ;
		    }
	        } /* end for */
	        rs = (i < op->i) ? SR_OK : SR_NOTFOUND ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecitem_find) */

int vecitem_getvec(vecitem *op,void **rpp) noex {
	int		rs = SR_FAULT ;
	if (op && rpp) {
	    rs = op->c ;
	    *rpp = op->va ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecitem_getvec) */

int vecitem_audit(vecitem *op) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) {
	    rs = SR_OK ;
	    for (int i = 0 ; i < op->i ; i += 1) {
	        void	*ep = op->va[i] ;
	        int		*ip ;
	        if (ep) {
	            c += 1 ;
	            ip = (int *) op->va[i] ;
	            rs |= *ip ;		/* access might SEGFAULT */
	        }
	    } /* end for */
	    rs = (c == op->c) ? SR_OK : SR_BADFMT ;
	} /* end if (valid) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecitem_audit) */


/* private subroutines */

static int vecitem_setopts(vecitem *op,int vo) noex {
	constexpr int	m = mkoptmask() ;
	int		rs = SR_INVALID ;
	op->f = {} ;
	if ((vo & (~m)) == 0) {
	    rs = SR_OK ;
	    if (vo & VECITEM_OREUSE) op->f.oreuse = 1 ;
	    if (vo & VECITEM_OSWAP) op->f.oswap = 1 ;
	    if (vo & VECITEM_OSTATIONARY) op->f.ostationary = 1 ;
	    if (vo & VECITEM_OCOMPACT) op->f.ocompact = 1 ;
	    if (vo & VECITEM_OSORTED) op->f.osorted = 1 ;
	    if (vo & VECITEM_OORDERED) op->f.oordered = 1 ;
	    if (vo & VECITEM_OCONSERVE) op->f.oconserve = 1 ;
	} /* end if (valid options) */
	return rs ;
}
/* end subroutine (vecitem_setopts) */

static int vecitem_extend(vecitem *op) noex {
	int		rs = SR_OK ;
	if ((op->i + 1) > op->n) {
	    cint	esize = sizeof(char **) ;
	    int		nn, size ;
	    void	*np{} ;
	    if (op->va == nullptr) {
	        nn = VECITEM_DEFENTS ;
	        size = (nn + 1) * esize ;
	        rs = uc_malloc(size,&np) ;
	    } else {
	        nn = (op->n + 1) * 2 ;
	        size = (nn + 1) * esize ;
	        rs = uc_realloc(op->va,size,&np) ;
	        op->va = nullptr ;
	    }
	    if (rs >= 0) {
	        op->va = (void **) np ;
	        op->n = nn ;
	    }
	} /* end if (extension required) */
	return rs ;
}
/* end subroutine (vecitem_extend) */

static int vecitem_iget(vecitem *op,int i,void **gpp) noex {
	int		rs = SR_FAULT ;
	if (op && gpp) {
	    rs = SR_NOTFOUND ;
	    *gpp = nullptr ;
	    if ((i >= 0) && (i < op->i)) {
		rs = SR_OK ;
	        *gpp = (op->va)[i] ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecitem_iget) */

static int vecitem_fetchbeg(vecitem *op,fetchargs *ap) noex {
	int		rs ;
	cvoid		*cep = ap->cep ;
	cur		*curp = ap->curp ;
	cmpf		cf = ap->cf ;
	void		**rpp = ap->rpp ;
	int		i = 0 ;
	curp->c = 0 ;
	if ((rs = vecitem_search(op,cep,cf,rpp)) > 0) {
	    auto	vig = vecitem_iget ;
	    int		j{} ;
	    i = rs ;
            if (op->f.osorted) {
                void    *last = *rpp ;
                cint    js = (rs - 1) ;
                auto    lamb = [&]() noex {
                    return (j >= 0) && ((rs = vig(op,j,rpp)) >= 0) ; 
                } ;
                for (j = js ; lamb() ; j -= 1) {
                    if (cf(&cep,cvoidpp(rpp)) != 0) break ;
                    last = *rpp ;
                } /* end for */
                curp->i = (j + 1) ;
                *rpp = last ;
		i = curp->i ;
                rs = curp->i ;
            } else {
                curp->i = rs ;
            }
	} /* end if (search) */
	if (rs >= 0) {
	    curp->i = i ;
	    curp->c = 1 ;
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecitem_fetchbeg) */

static int vecitem_fetchcont(vecitem *op,fetchargs *ap) noex {
	int		rs = SR_OK ;
	cvoid		*cep = ap->cep ;
	cur		*curp = ap->curp ;
	cmpf		cf = ap->cf ;
	void		**rpp = ap->rpp ;
	void		*tep{} ;
	int		i = 0 ;
        if (op->f.osorted) {
            if (! op->f.issorted) {
                op->f.issorted = true ;
                if (op->c > 1) {
                    qsort_f     scf = qsort_f(cf) ;
                    cint        ssz = sizeof(void *) ;
                    qsort(op->va,op->i,ssz,scf) ;
                } /* end if (sorting) */
                if ((rs = vecitem_search(op,cep,cf,rpp)) >= 0) {
                    cint        is = rs ;
                    int         j{} ;
                    i = rs ;
                    if (is > 0) {
                        auto    vig = vecitem_get ;
                        cint    js = (is - 1) ;
                        void    *last = *rpp ;
                        auto	lamb = [&]() noex {
                            bool        f = true ;
                            f = f && (j >= 0) ;
                            f = f && ((rs = vig(op,j,rpp)) >= 0) ; 
                            return f ;
                        } ;
                        for (j = js ; lamb() ; j -= 1) {
                            if ((*cf)(&cep,cvoidpp(rpp)) != 0) break ;
                            last = *rpp ;
                        } /* end for */
                        i = (j + 1) ;
                        *rpp = last ;
                    } /* end if */
/* skip up to 'cp->c' count minus one number of matches */
                    i += (curp->c - 1) ;
                    curp->i = i ;
                } else {
                    curp->i = op->i ;
                }
            } /* end if (it was out-of-order) */
/* return the next one */
            i = (curp->i + 1) ;
	    if (rs >= 0) {
                if ((rs = vecitem_iget(op,i,&tep)) >= 0) {
                    cvoid   *ctep = cvoidp(tep) ;
                    if ((*cf)(&cep,&ctep) != 0) {
                        rs = SR_NOTFOUND ;
                    }
                } /* end if (vecitem_iget) */
	    } /* end if (ok) */
        } else {
            cint        is = (curp->i + 1) ;
            auto	lamb = [&]() noex {
                return ((rs = vecitem_iget(op,i,&tep)) >= 0) ;
            } ;
            for (i = is ; lamb() ; i += 1) {
                if (tep) {
                    cvoid       *ctep = cvoidp(tep) ;
                    if ((*cf)(&cep,&ctep) == 0) break ;
                }
            } /* end for */
        } /* end if (sorted policy or not) */
        if (rs >= 0) {
            curp->i = i ;
            curp->c += 1 ;
            if (rpp) *rpp = tep ;
        }
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecitem_fetchcont) */



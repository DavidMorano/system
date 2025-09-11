/* vecitem SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* vector-item operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	vecitem

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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<localmisc.h>

#include	"vecitem.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| + |memcopy(3u)| */

/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */

extern "C" {
    typedef int (*qsort_f)(cvoid *,cvoid *) noex ;
    typedef vecitem_cmpf	cmpf ;
}

typedef vecitem_cur	cur ;


/* external subroutines */


/* external variables */


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

static int	vecitem_setopts(vecitem *,int) noex ;
static int	vecitem_extend(vecitem *) noex ;
static int	vecitem_iget(vecitem *,int,void **) noex ;
static int	vecitem_fetchbeg(vecitem *,fetchargs *) noex ;
static int	vecitem_fetchcont(vecitem *,fetchargs *) noex ;

consteval cur mkcurdef() noex {
	cur	c{} ;
	c.c = 0 ;
	c.i = -1 ;
	return c ;
} /* end subroutine (mkcurdef) */


/* local variables */

constexpr cur		curdef = mkcurdef() ;


/* exported variables */


/* exported subroutines */

int vecitem_start(vecitem *op,int n,int opts) noex {
	VECITEM		*hop = op ;
	int		rs = SR_FAULT ;
	if (n < 0) n = VECITEM_DEFENTS ;
	if (op) {
	    memclear(hop) ;	/* <- potentially dangerous if type changes */
	    if ((rs = vecitem_setopts(op,opts)) >= 0) {
	        cint	sz = (n + 1) * szof(char **) ;
		if (void *vp ; (rs = libmem.mall(sz,&vp)) >= 0) {
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
	            rs1 = libmem.free(ep) ;
	            if (rs >= 0) rs = rs1 ;
		    op->va[i] = nullptr ;
	        }
	    } /* end for */
	    {
	        rs1 = libmem.free(op->va) ;
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

int vecitem_add(vecitem *op,cvoid *ep,int el) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && ep) {
	    bool	f_done = false ;
	    bool	f ;
	    if (el < 0) el = lenstr(charp(ep)) ;
	    if (char *bp ; (rs = libmem.mall((el+1),&bp)) >= 0) {
	        memcopy(bp,ep,el) ;
	        bp[el] = '\0' ;
	        f = (op->fl.oreuse || op->fl.oconserve) && (! op->fl.oordered) ;
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
	                libmem.free(bp) ;
	            }
	        }
	        op->fl.issorted = false ;
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
	            libmem.free(op->va[i]) ;
	        } /* end if */
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
	            if ((op->fl.oswap || op->fl.ocompact) && (i < (op->i - 1))) {
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
	    if (! op->fl.issorted) {
	        op->fl.issorted = true ;
	        if (op->c > 1) {
		    qsort_f	scf = qsort_f(cf) ;
		    cint	ssz = szof(void *) ;
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
	    cnullptr	np{} ;
	    rs = SR_OK ;
	    if (op->fl.osorted && (! op->fl.issorted)) {
	        op->fl.issorted = true ;
	        if (op->c > 1) {
		    qsort_f	scf = qsort_f(cf) ;
		    cint	ssz = szof(void *) ;
	            qsort(op->va,op->i,ssz,scf) ;
	        }
	    } /* end if (sorting) */
	    if (op->fl.issorted) {
	        qsort_f		scf = qsort_f(cf) ;
	        cint		ssz = szof(void *) ;
	        void		**spp ;
	        rs = SR_NOTFOUND ;
	        if ((spp = voidpp(bsearch(&cep,op->va,op->i,ssz,scf))) != np) {
	            i = intconv(spp - op->va) ;
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

static int vecitem_setopts(vecitem *op,int vo) noex {
	constexpr int	m = mkoptmask() ;
	int		rs = SR_INVALID ;
	op->fl = {} ;
	if ((vo & (~ m)) == 0) {
	    rs = SR_OK ;
	    if (vo & VECITEM_OREUSE) op->fl.oreuse = 1 ;
	    if (vo & VECITEM_OSWAP) op->fl.oswap = 1 ;
	    if (vo & VECITEM_OSTATIONARY) op->fl.ostationary = 1 ;
	    if (vo & VECITEM_OCOMPACT) op->fl.ocompact = 1 ;
	    if (vo & VECITEM_OSORTED) op->fl.osorted = 1 ;
	    if (vo & VECITEM_OORDERED) op->fl.oordered = 1 ;
	    if (vo & VECITEM_OCONSERVE) op->fl.oconserve = 1 ;
	} /* end if (valid options) */
	return rs ;
}
/* end subroutine (vecitem_setopts) */

static int vecitem_extend(vecitem *op) noex {
	int		rs = SR_OK ;
	if ((op->i + 1) > op->n) {
	    cint	esize = szof(char **) ;
	    int		nn ;
	    int		sz ;
	    void	*na{} ;
	    if (op->va == nullptr) {
	        nn = VECITEM_DEFENTS ;
	        sz = (nn + 1) * esize ;
	        rs = libmem.mall(sz,&na) ;
	    } else {
	        nn = (op->n + 1) * 2 ;
	        sz = (nn + 1) * esize ;
	        rs = libmem.rall(op->va,sz,&na) ;
	        op->va = nullptr ;
	    }
	    if (rs >= 0) {
	        op->va = (void **) na ;
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
            if (op->fl.osorted) {
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
        if (op->fl.osorted) {
            if (! op->fl.issorted) {
                op->fl.issorted = true ;
                if (op->c > 1) {
                    qsort_f     scf = qsort_f(cf) ;
                    cint        ssz = szof(void *) ;
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

int vecitem::start(int an,int ao) noex {
	return vecitem_start(this,an,ao) ;
}

int vecitem::add(cvoid *ep,int el) noex {
	return vecitem_add(this,ep,el) ;
}

int vecitem::get(int ai,void *rp) noex {
	return vecitem_get(this,ai,rp) ;
}

int vecitem::find(cvoid *ep,int el) noex {
    	return vecitem_find(this,ep,el) ;
}

int vecitem::fetch(cvoid *ep,vecitem_cur *curp,vecitem_cmpf cf,void *rp) noex {
    	return vecitem_fetch(this,ep,curp,cf,rp) ;
}

int vecitem::search(cvoid *ep,vecitem_cmpf cf,void *rp) noex {
    	return vecitem_search(this,ep,cf,rp) ;
}

int vecitem::sort(vecitem_cmpf cf) noex {
    	return vecitem_sort(this,cf) ;
}

int vecitem::getvec(void **rpp) noex {
	return vecitem_getvec(this,rpp) ;
}

void vecitem::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("vecitem",rs,"fini-finish") ;
	}
}

vecitem::operator int () noex {
    	return vecitem_count(this) ;
}

int vecitem_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case vecitemmem_del:
	        rs = vecitem_del(op,a) ;
	        break ;
	    case vecitemmem_count:
	        rs = vecitem_count(op) ;
	        break ;
	    case vecitemmem_audit:
	        rs = vecitem_audit(op) ;
	        break ;
	    case vecitemmem_finish:
	        rs = vecitem_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (vecitem_co::operator) */

bool vecitem_iter::operator == (const vecitem_iter &oit) noex {
	return (va == oit.va) && (i == oit.i) && (ii == oit.ii) ;
}

bool vecitem_iter::operator != (const vecitem_iter &oit) noex {
	bool		f = false ;
	f = f || (va != oit.va) ;
	f = f || (ii != oit.ii) ;
	if (!f) {
	    f = (i < oit.i) ;
	}
	return f ;
}
/* end method (vecitem_iter::operator) */

vecitem_iter vecitem_iter::operator + (int n) const noex {
	vecitem_iter	rit(va,i,i) ;
	rit.i = ((rit.i + n) >= 0) ? (rit.i + n) : 0 ;
	return rit ;
}

vecitem_iter vecitem_iter::operator += (int n) noex {
	vecitem_iter	rit(va,i,i) ;
	i = ((i + n) >= 0) ? (i + n) : 0 ;
	rit.i = i ;
	return rit ;
}

vecitem_iter vecitem_iter::operator ++ () noex { /* pre */
	increment() ;
	return (*this) ;
}

vecitem_iter vecitem_iter::operator ++ (int) noex { /* post */
	vecitem_iter	pre(*this) ;
	increment() ;
	return pre ;
}

void vecitem_iter::increment(int n) noex {
	if ((i + n) < 0) n = -i ;
	if (n != 0) {
	    i += n ;
	    while ((i < ii) && (va[i] == nullptr)) {
	        i += 1 ;
	    }
	}
}
/* end method (vecitem_iter::increment) */



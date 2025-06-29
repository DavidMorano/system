/* vecsorthand SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* vector of sorted handles */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-28, David A-D- Morano
	Module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	vecsorthand

	Description:
	This object is used when the caller just wants to store an
	item in a sorted (flat) list.  This object is a vector-type
	object that stores pointers to the caller data objects to
	be stored.  But the list is maintained in a sorted order
	(using a comparison subroutine given by the caller).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

#include	"vecsorthand.h"


/* local defines */

#define	VECSORTHAND_DEFENTS	10


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef vecsorthand_cmpf		cmp_f ; /* Cmp-Function */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int vecsorthand_ctor(vecsorthand *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_OK ;
	    op->va = nullptr ;
	    op->c = 0 ;
	    op->i = 0 ;
	    op->e = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecsorthand_ctor) */

static int	vecsorthand_extend(vecsorthand *) noex ;
static int	topidx(int) noex ;


/* local variables */

cint		defents = VECSORTHAND_DEFENTS ;


/* exported variables */


/* exported subroutines */

int vecsorthand_start(vecsorthand *op,cmp_f cmpfunc,int vn) noex {
	int		rs ;
	if (vn <= 1) vn = defents ;
	if ((rs = vecsorthand_ctor(op,cmpfunc)) >= 0) {
	    cint	sz = (szof(void **) * (vn + 1)) ;
	    if (void *vp{} ; (rs = uc_libmalloc(sz,&vp)) >= 0) {
	        op->va = voidpp(vp) ;
	        op->e = vn ;
	        {
	            op->va[0] = nullptr ;
	            op->cmpf = cmpfunc ;
	        }
	    } /* end if (m-a) */
	} /* end if (vecsorthand_ctor) */
	return rs ;
}
/* end subroutine (vecsorthand_start) */

int vecsorthand_finish(vecsorthand *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->va) {
	        rs1 = uc_libfree(op->va) ;
	        if (rs >= 0) rs = rs1 ;
	        op->va = nullptr ;
	    }
	    op->c = 0 ;
	    op->i = 0 ;
	    op->e = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecsorthand_finish) */

int vecsorthand_add(vecsorthand *op,cvoid *nep) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && nep) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
	        if ((rs = vecsorthand_extend(op)) >= 0) {
		    if (op->i > 0) {
		        auto	cf = op->cmpf ;
			int	rc = -1 ;
	                int	bot = 0 ;
	                int	top = topidx(op->i) ;
	                i = (bot + top) / 2 ;
	                while ((top - bot) > 0) {
	                    if ((rc = cf(nep,op->va[i])) < 0) {
	                        top = i - 1 ;
	                    } else if (rc > 0) {
	                        bot = i + 1 ;
	                    } else {
	                        break ;
		            }
	                    i = (bot + top) / 2 ;
	                } /* end while */
	                if (i < op->i) {
	                    if ((rc != 0) && (cf(nep,op->va[i]) > 0)) {
	                        i += 1 ;
	                    }
	                    for (int j = (op->i - 1) ; j >= i ; j -= 1) {
	                        op->va[j + 1] = op->va[j] ;
	                    }
	                } /* end if */
		    } /* end if (non-zero positive) */
	            op->va[i] = voidpp(nep) ;
	            op->i += 1 ;
	            op->va[op->i] = nullptr ;
	            op->c += 1 ;		/* increment list count */
	        } /* end if (vecsorthand_extend) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecsorthand_add) */

int vecsorthand_get(vecsorthand *op,int i,void *vp) noex {
	int		rs = SR_NOTFOUND ;
	if (op && vp) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		void		*rval = nullptr ;
		rs = SR_NOTFOUND ;
		if ((i >= 0) && (i < op->i)) {
	    	    rval = op->va[i] ;
	    	    rs = i ;
		}
	        if (vp) {
	            void	**rpp = voidpp(vp) ;
	            *rpp = rval ;
	        }
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecsorthand_get) */

int vecsorthand_del(vecsorthand *op,int i) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = SR_NOTFOUND ;
	        if ((i >= 0) && (i < op->i)) {
	            op->i -= 1 ;
	            for (int j = i ; j < op->i ; j += 1) {
	                op->va[j] = op->va[j + 1] ;
	            }
	            op->va[op->i] = nullptr ;
	            op->c -= 1 ;		/* decrement list count */
	            rs = op->c ;
	        } /* end if */
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecsorthand_del) */

int vecsorthand_delhand(vecsorthand *op,cvoid *ep) noex {
	int		rs = SR_FAULT ;
	if (op && ep) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
	        cint	n = op->i ;
	        int	i ; /* used-afterwards */
	        bool	f = false ;
	        rs = SR_NOTFOUND ;
	        for (i = 0 ; i < n ; i += 1) {
	            f = (op->va[i] == ep) ;
	            if (f) break ;
	        } /* end for */
	        if (f) {
	            rs = vecsorthand_del(op,i) ;
	        }
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecsorthand_delhand) */

int vecsorthand_count(vecsorthand *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = op->c ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecsorthand_count) */

int vecsorthand_search(vecsorthand *op,cvoid *ep,void *vrp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && ep) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = SR_NOTFOUND ;
		if (op->i > 0) {
		    auto	cf = op->cmpf ;
		    int		rc = -1 ;
	            int		bot = 0 ;
	            int		top = topidx(op->i) ;
	            i = (bot + top) / 2 ;
		    auto lamb = [&op,&cf,&ep] (int ii) noex {
			return cf(ep,op->va[ii]) ;
		    } ;
		    while (((top - bot) > 0) && ((rc = lamb(i)) != 0)) {
		        if (rc < 0) {
	                    top = i - 1 ;
	                } else {
	                    bot = i + 1 ;
		        }
	                i = (bot + top) / 2 ;
	            } /* end while */
	            if (rc == 0) {
			rs = SR_OK ;
		    } else if (i < op->i) {
	                if (cf(ep,op->va[i]) == 0) rs = SR_OK ;
		    }
		} /* end if (have entries) */
		if (vrp) {
	    	    void	**rpp = voidpp(vrp) ;
	    	    *rpp = ((rs >= 0) ? op->va[i] : nullptr) ;
		}
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecsorthand_search) */


/* private subroutines */

static int vecsorthand_extend(vecsorthand *op) noex {
	int		rs = SR_OK ;
	if ((op->i + 1) > op->e) {
	    cint	ndef = defents ;
	    int		ne ;
	    int		sz ;
	    void	**nva{} ;
	    if (op->e == 0) {
	        ne = ndef ;
	        sz = (ne * szof(void **)) ;
	        rs = uc_libmalloc(sz,&nva) ;
	    } else {
	        ne = (op->e * 2) ;
	        sz = (ne * szof(void **)) ;
	        rs = uc_librealloc(op->va,sz,&nva) ;
	    }
	    if (rs >= 0) {
	        op->va = nva ;
	        op->e = ne ;
		op->va[op->i] = nullptr ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (vecsorthand_extend) */

int vecsorthand::start(cmp_f cf,int vn) noex {
	return vecsorthand_start(this,cf,vn) ;
}

int vecsorthand::add(cvoid *nep) noex {
	return vecsorthand_add(this,nep) ;
}

int vecsorthand::get(int ai,void *rvp) noex {
	return vecsorthand_get(this,ai,rvp) ;
}

void vecsorthand::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("vecsorthand",rs,"fini-finish") ;
	}
}

vecsorthand::operator int () noex {
	int		rs = SR_NOTOPEN ;
	if (cmpf) {
	    rs = c ;
	}
	return rs ;
}

int vecsorthand_co::operator () (int ai) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case vecsorthandmem_count:
	        rs = vecsorthand_count(op) ;
	        break ;
	    case vecsorthandmem_del:
	        rs = vecsorthand_del(op,ai) ;
	        break ;
	    case vecsorthandmem_finish:
	        rs = vecsorthand_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (vecsorthand_co::operator) */

static int topidx(int i) noex {
    	if (i > 0) {
	    i -= 1 ;
	}
	return i ;
}
/* end subroutine (topidx) */



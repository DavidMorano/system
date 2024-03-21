/* vecsorthand SUPPORT */
/* lang=C++20 */

/* vector of sorted handles */
/* version %I% last-modified %G% */


/* revision history:

	= 2010-12-01,  David A.D. Morano
	Module was originally written.

*/

/* Copyright © 2011 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	These routines are used when the caller just wants to store
	an item in a sorted list.  The item us usually just a pointer
	to the user's real data.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<cstdlib>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"vecsorthand.h"


/* local defines */

#define	VECSORTHAND_DEFENTS	10


/* external subroutines */


/* forward references */

static int	vecsorthand_ctor(vecsorthand *) noex ;
static int	vecsorthand_extend(vecsorthand *) noex ;

extern "C" {
    static int	defcmpfunc(cvoid *,cvoid *) noex ;
}


/* exported variables */


/* exported subroutines */

int vecsorthand_start(vecsorthand *op,int n,vecentcmp_f cmpfunc) noex {
	int		rs = SR_FAULT ;
	if (n <= 1) n = VECSORTHAND_DEFENTS ;
	if ((rs = vecsorthand_ctor(op)) >= 0) {
	    int		size = 0 ;
	    void	*vp{} ;
	    size += (sizeof(void **) * (n+1)) ;
	    if ((rs = uc_libmalloc(size,&vp)) >= 0) {
	        op->va = (void **) vp ;
	        op->e = n ;
	        {
	            op->va[0] = nullptr ;
	            op->cmpfunc = (cmpfunc) ? cmpfunc : defcmpfunc ;
	        }
	    } /* end if (m-a) */
	} /* end if (non-null) */
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
	    op->i = 0 ;
	    op->e = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (vecsorthand_finish) */

int vecsorthand_add(vecsorthand *op,cvoid *buf) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && buf) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
	        if ((rs = vecsorthand_extend(op)) >= 0) {
	            int		bot, top ;
	            op->c += 1 ;		/* increment list count */
	            bot = 0 ;
	            top = MAX((op->i - 1),0) ;
	            i = (bot + top) / 2 ;
	            while ((top - bot) > 0) {
	                if ((rs = (*op->cmpfunc)(&buf,(op->va+i))) < 0) {
	                    top = i - 1 ;
	                } else if (rs > 0) {
	                    bot = i + 1 ;
	                } else {
	                    break ;
		        }
	                i = (bot + top) / 2 ;
	            } /* end while */
	            if (i < op->i) {
	                if ((*op->cmpfunc)(&buf,&op->va[i]) > 0) {
	                    i += 1 ;
	                }
	                for (int j = (op->i - 1) ; j >= i ; j -= 1) {
	                    op->va[j + 1] = op->va[j] ;
	                }
	            } /* end if */
	            op->va[i] = (void *) buf ;
	            op->i += 1 ;
	            op->va[op->i] = nullptr ;
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
		cvoid		*rval = nullptr ;
		rs = SR_NOTFOUND ;
		if ((i >= 0) && (i < op->i)) {
	    	    rval = op->va[i] ;
	    	    rs = i ;
		}
	        if (vp) {
	            cvoid	**rpp = (cvoid **) vp ;
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
	        int	i ;
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
	cint		esize = sizeof(void *) ;
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && ep) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		vecentcmp_f	cf = op->cmpfunc ;
		void		**spp ;
		rs = SR_NOTFOUND ;
		spp = (void **) bsearch(&ep,op->va,op->i,esize,cf) ;
		if (spp) {
	    	    i = (spp - op->va) ;
	    	    rs = SR_OK ;
		}
		if (vrp) {
	    	    void	**rpp = (void **) vrp ;
	    	    *rpp = ((rs >= 0) ? op->va[i] : nullptr) ;
		}
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecsorthand_search) */


/* private subroutines */

static int vecsorthand_ctor(vecsorthand *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->va = nullptr ;
	    op->c = 0 ;
	    op->i = 0 ;
	    op->e = 0 ;
	}
	return rs ;
}
/* end subroutine (vecsorthand_ctor) */

static int vecsorthand_extend(vecsorthand *op) noex {
	int		rs = SR_OK ;
	if ((op->i + 1) > op->e) {
	    cint	ndef = VECSORTHAND_DEFENTS ;
	    int		size ;
	    int		ne ;
	    void	**np{} ;
	    if (op->e == 0) {
	        ne = ndef ;
	        size = (ndef*sizeof(void **)) ;
	        rs = uc_libmalloc(size,&np) ;
	    } else {
	        cint	ne = (op->e * 2) ;
	        size = (ne*sizeof(void **)) ;
	        rs = uc_librealloc(op->va,size,&np) ;
	    }
	    if (rs >= 0) {
	        op->va = np ;
	        op->e = ne ;
		op->va[op->i] = nullptr ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (vecsorthand_extend) */

static int defcmpfunc(cvoid *app,cvoid *bpp) noex {
	cchar	**e1pp = (cchar **) app ;
	cchar	**e2pp = (cchar **) bpp ;
	return strcmp(*e1pp,*e2pp) ;
}
/* end subroutine (defcmpfunc) */



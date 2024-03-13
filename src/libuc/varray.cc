/* varray SUPPORT */
/* lang=C++20 */

/* object implementing variable-length array of elements */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This module was originally written for hardware CAD support.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object attempts to implement a sort of variable-length
	array of elements.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<lookaside.h>
#include	<localmisc.h>

#include	"varray.h"


/* local defines */

#define	OURMALLOC(size,pointer)		uc_libmalloc((size),(pointer))
#define	OURREALLOC(p1,size,p2)		uc_librealloc((p1),(size),(p2))
#define	OURFREE(pointer)		uc_libfree((pointer))


/* imported namespaces */

using std::nothrow ;


/* local typedefs */


/* external subroutines */

int		varray_search(varray *,void *,varray_vcmp,void *) noex ;


/* local structures */


/* forward references */

template<typename ... Args>
static inline int varray_ctor(varray *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    op->va = nullptr ;
	    op->esize = 0 ;
	    op->c = 0 ;
	    op->n = 0 ;
	    op->imax = 0 ;
	    if ((op->lap = new(nothrow) lookaside) != nullptr) {
		rs = SR_OK ;
	    } /* end if (new-lookaside) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (varray_ctor) */

static inline int varray_dtor(varray *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->lap) {
		delete op->lap ;
		op->lap = nullptr ;
	    }
	}
	return rs ;
}
/* end subroutine (varray_dtor) */

static int	varray_extend(varray *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int varray_start(varray *op,int esize,int n) noex {
	int		rs ;
	if ((rs = varray_ctor(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (n <= 0) n = VARRAY_DEFENTS ;
	    if (esize > 0) {
	        cint	size = (n + 1) * sizeof(void **) ;
	        void	*vp{} ;
	        op->esize = esize ;
	        if ((rs = OURMALLOC(size,&vp)) >= 0) {
	            memclear(vp,size) ;
	            op->va = (void **) vp ;
	            op->n = n ;
	            rs = lookaside_start(op->lap,esize,n) ;
	            if (rs < 0) {
	                OURFREE(vp) ;
		    }
	        } /* end if (m-a) */
	    } /* end if (valid) */
	    if (rs < 0) {
		varray_dtor(op) ;
	    }
	} /* end if (varray_ctor) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (varray_start) */

int varray_finish(varray *op) noex {
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
	            rs1 = OURFREE(op->va) ;
	            if (rs >= 0) rs = rs1 ;
	            op->va = nullptr ;
		}
	        {
		    rs1 = varray_dtor(op) ;
	            if (rs >= 0) rs = rs1 ;
		}
	        op->c = 0 ;
		op->n = 0 ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (varray_finish) */

int varray_enum(varray *op,int i,void *rp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = SR_INVALID ;
	        if (i >= 0) {
		    rs = SR_NOTFOUND ;
	            if (i < (op->imax+1)) {
	                if (op->va[i]) rs = 1 ;
	            } 
	            if (rp) {
	                void	**rpp = (void **) rp ;
	                *rpp = (rs >= 0) ? op->va[i] : nullptr ;
	            } /* end if (response wanted) */
		} /* end if (valid) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (varray_enum) */

int varray_acc(varray *op,int i,void *rp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = SR_INVALID ;
	        if (i >= 0) {
		    rs = SR_NOTFOUND ;
		    void	*ep = nullptr ;
	            if (i < op->n) {
	                ep = (op->va)[i] ;
	                rs = (ep != nullptr) ;
	            }
	            if (rp) {
	                void	**rpp = (void **) rp ;
	                *rpp = ep ;
	            }
		} /* end if (valid) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (varray_acc) */

int varray_mk(varray *op,int i,void *rp) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = SR_INVALID ;
	        if (i >= 0) {
		    rs = SR_OK ;
	            if (i >= op->n) {
	                rs = varray_extend(op,i) ;
	            }
	            if ((rs >= 0) && op->va[i]) {
	                void	*ep ;
	                if ((rs = lookaside_get(op->lap,&ep)) >= 0) {
	                    if (i > op->imax) op->imax = i ;
	                    op->c += 1 ;
	                    op->va[i] = ep ;
			    c = op->c ;
	                }
	            } /* end if */
	            if (rp) {
	                void	**rpp = (void **) rp ;
	                *rpp = (rs >= 0) ? (op->va)[i] : nullptr ;
	            }
		} /* end if (valid) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (varray_mk) */

int varray_del(varray *op,int i) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = SR_INVALID ;
	        if ((i > 0) && (i < op->n)) {
		    void	*ep = op->va[i] ;
	            if (ep) {
	                if ((rs = lookaside_release(op->lap,ep)) >= 0) {
	                    op->va[i] = nullptr ;
	                    op->c -= 1 ;
		            c = op->c ;
	                }
	            }
		} /* end if (valid) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (varray_del) */

int varray_delall(varray *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = SR_OK ;
	        for (int i = 0 ; i < op->n ; i += 1) {
		    void	*ep = op->va[i] ;
	    	    if (ep) {
			rs1 = lookaside_release(op->lap,ep) ;
			if (rs >= 0) rs = rs1 ;
			op->va[i] = nullptr ;
	    	    }
		} /* end for */
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (varray_delall) */

int varray_count(varray *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		rs = op->c ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (varray_count) */

int varray_search(varray *op,void *oep,varray_vcmp fvcmp,void *vrp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && oep && fvcmp) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		void	**rpp = (void **) vrp ;
		cvoid	*cep = (cvoid *) oep ;
	        for (i = 0 ; i < op->n ; i += 1) {
	            if (op->va[i]) {
			cvoid	**cva = (cvoid **) (op->va+i) ;
	                if (fvcmp(&cep,cva) == 0) break ;
	            }
	        } /* end for */
	        rs = (i < op->n) ? SR_OK : SR_NOTFOUND ;
	        if (rpp) {
	            *rpp = (rs >= 0) ? op->va[i] : nullptr ;
	        }
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (varray_search) */

int varray_find(varray *op,void *oep) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && oep) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
		cint	esz = op->esize ;
		for (i = 0 ; i < op->n ; i += 1) {
		    void	*ep = op->va[i] ;
	            if (ep) {
	                if (memcmp(oep,ep,esz) == 0) break ;
		    }
	        } /* end for */
	        rs = (i < op->n) ? SR_OK : SR_NOTFOUND ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (varray_find) */

int varray_audit(varray *op) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->va) {
	        for (int i = 0 ; i < op->n ; i += 1) {
	            if (op->va[i]) {
	                cint	*ip = (int *) op->va[i] ;
	                c += 1 ;
	                rs |= *ip ;		/* access might SEGFAULT */
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
/* end subroutine (varray_audit) */


/* local subroutines */

static int varray_extend(VARRAY *op,int ni) noex {
	int		rs = SR_OK ;
	if (ni >= op->n) {
	    cint	ninc = VARRAY_DEFENTS ;
	    cint	ndif = ((ni+1)-op->n) ;
	    int		nn, size ;
	    void	*np{} ;
	    nn = (op->n + MAX(ndif,ninc)) ;
	    size = nn * sizeof(void **) ;
	    if (op->va == nullptr) {
	        if ((rs = OURMALLOC(size,&np)) >= 0) {
	            memclear(np,size) ;
	        }
	    } else {
	        if ((rs = OURREALLOC(op->va,size,&np)) >= 0) {
	            void	**nva = (void **) np ;
	            cint	nndif = (nn-op->n) ;
	            int		dsize ;
	            dsize = (nndif * sizeof(void **)) ;
	            memclear((nva+op->n),dsize) ;
	            op->va = nullptr ;
	        }
	    } /* end if */
	    if (rs >= 0) {
	        op->va = (void **) np ;
	        op->n = nn ;
	    }
	} /* end if (reallocation needed) */
	return rs ;
}
/* end subroutine (varray_extend) */



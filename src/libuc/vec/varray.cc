/* varray SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* object implementing variable-length array of elements */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This module was originally written for hardware CAD support.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	varray

	Description:
	This object attempts to implement a sort of variable-length
	array of elements.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSYD */
#include	<cstdlib>		/* CSYD */
#include	<new>			/* C++STD |nothrow(3c++) */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<lookaside.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"varray.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local inline int varray_ctor(varray *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    op->va = nullptr ;
	    op->esz = 0 ;
	    op->c = 0 ;
	    op->n = 0 ;
	    op->imax = 0 ;
	    if ((op->lap = new(nothrow) lookaside) != nullptr) ylikely {
		rs = SR_OK ;
	    } /* end if (new-lookaside) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (varray_ctor) */

local inline int varray_dtor(varray *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->lap) ylikely {
		delete op->lap ;
		op->lap = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (varray_dtor) */

local int	varray_extend(varray *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int varray_start(varray *op,int esz,int n) noex {
	int		rs ;
	if ((rs = varray_ctor(op)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (n <= 0) n = VARRAY_DEFENTS ;
	    if (esz > 0) ylikely {
	        cint	sz = (n + 1) * szof(void **) ;
	        op->esz = esz ;
	        if (void *vp ; (rs = libmem.mall(sz,&vp)) >= 0) ylikely {
	            memclear(vp,sz) ;
	            op->va = (void **) vp ;
	            op->n = n ;
	            rs = lookaside_start(op->lap,esz,n) ;
	            if (rs < 0) {
	                libmem.free(vp) ;
			op->va = nullptr ;
		    } /* end if (error) */
	        } /* end if (memory-acquire) */
	    } /* end if (valid) */
	    if (rs < 0) {
		varray_dtor(op) ;
	    } /* end if (error) */
	} /* end if (varray_ctor) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (varray_start) */

int varray_finish(varray *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->va) ylikely {
		rs = SR_OK ;
	        if (op->lap) ylikely {
		    rs1 = lookaside_finish(op->lap) ;
		    if (rs >= 0) rs = rs1 ;
		}
		if (op->va) ylikely {
	            rs1 = libmem.free(op->va) ;
	            if (rs >= 0) rs = rs1 ;
	            op->va = nullptr ;
		} /* end if (memory-release) */
	    } /* end if (open) */
	    {
		rs1 = varray_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->c = 0 ;
	    op->n = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (varray_finish) */

int varray_enum(varray *op,int i,void *rp) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->va) ylikely {
		rs = SR_INVALID ;
	        if (i >= 0) ylikely {
		    rs = SR_NOTFOUND ;
	            if (i < (op->imax + 1)) {
	                if (op->va[i]) rs = 1 ; /* <- return-status */
	            } /* end if */
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
	if (op) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->va) ylikely {
		rs = SR_INVALID ;
	        if (i >= 0) ylikely {
		    rs = SR_NOTFOUND ;
		    void	*ep = nullptr ;
	            if (i < op->n) {
	                ep = (op->va)[i] ;
	                rs = (ep != nullptr) ; /* <- return-status */
	            } /* end if */
	            if (rp) {
	                void	**rpp = (void **) rp ;
	                *rpp = ep ;
	            } /* end if */
		} /* end if (valid) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (varray_acc) */

int varray_mk(varray *op,int i,void *rp) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->va) ylikely {
		rs = SR_INVALID ;
	        if (i >= 0) ylikely {
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
	if (op) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->va) ylikely {
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
	if (op) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->va) ylikely {
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
	if (op) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->va) ylikely {
		rs = op->c ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (varray_count) */

int varray_search(varray *op,void *oep,varray_vcmp fvcmp,void *vrp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (op && oep && fvcmp) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->va) ylikely {
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
	if (op && oep) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->va) ylikely {
		cint	esz = op->esz ;
		for (i = 0 ; i < op->n ; i += 1) {
		    void	*ep = op->va[i] ;
	            if (ep) {
			csize	esize = size_t(esz) ;
	                if (memcmp(oep,ep,esize) == 0) break ;
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
	if (op) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->va) ylikely {
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

local int varray_extend(varray *op,int ni) noex {
	int		rs = SR_OK ;
	if (ni >= op->n) ylikely {
	    cint	ninc = VARRAY_DEFENTS ;
	    cint	ndif = ((ni+1)-op->n) ;
	    int		nn ;
	    int		sz ;
	    void	*vp{} ; /* used-multiple */
	    nn = (op->n + MAX(ndif,ninc)) ;
	    sz = nn * szof(void **) ;
	    if (op->va == nullptr) {
	        if ((rs = libmem.mall(sz,&vp)) >= 0) {
	            memclear(vp,sz) ;
	        } /* end if (memory-acquire) */
	    } else {
	        if ((rs = libmem.rall(op->va,sz,&vp)) >= 0) {
	            void	**nva = (void **) vp ;
	            cint	nndif = (nn-op->n) ;
	            int		dsize ;
	            dsize = (nndif * szof(void **)) ;
	            memclear((nva+op->n),dsize) ;
	            op->va = nullptr ;
	        } /* end if (memory-acquire) */
	    } /* end if */
	    if (rs >= 0) {
	        op->va = (void **) vp ;
	        op->n = nn ;
	    }
	} /* end if (re-allocation needed) */
	return rs ;
} /* end subroutine (varray_extend) */



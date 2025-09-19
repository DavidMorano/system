/* fifoelem SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* FIFO string operations */
/* version %I% last-modified %G% */


/* revision history :

	= 1998-12-01, David A­D­ Morano
	Module was originally written.  This object may actually date 
	from the 1980s.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	fifoelem

	Description:
	This object manages a FIFO of fixed sized entries and of
	fixed sized depth.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strcmp(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<localmisc.h>

#include	"fifoelem.h"

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| + |memcopy(3u)| */

/* local defines */

#define	FE		fifoelem
#define	FE_ENT		fifoelem_ent
#define	FE_CUR		fifoelem_cur


/* imported namespace */

using libuc::libmem ;			/* variable */


/* local typedefs */

typedef const void	cv ;


/* external subroutines */


/* local structures */


/* forward referecens */

template<typename ... Args>
static inline int fifoelem_magic(fifoelem *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == FIFOELEM_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (fifoelem_magic) */

static int	entry_start(FE_ENT *,cvoid *,int) noex ;
static int	entry_finish(FE_ENT *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int fifoelem_start(FE *op) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->head = nullptr ;
	    op->tail = nullptr ;
	    op->n = 0 ;
	    op->magic = FIFOELEM_MAGIC ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fifoelem_start) */

int fifoelem_finish(FE *op) noex {
	int		rs ;
	if ((rs = fifoelem_magic(op)) >= 0) {
	    while ((rs = fifoelem_del(op)) >= 0) ;
	    if (rs == SR_NOTFOUND) rs = SR_OK ;
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fifoelem_finish) */

int fifoelem_add(FE *op,void *usp,int usl) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		n = 0 ; /* return-value */
	if ((rs = fifoelem_magic(op,usp)) >= 0) {
	    cint	rsz = szof(FE_ENT) ;
	    if (void *vp ; (rs = libmem.mall(rsz,&vp)) >= 0) {
	        FE_ENT	*ep = (FE_ENT *) vp ;
	        if ((rs = entry_start(ep,usp,usl)) >= 0) {
	            if (op->head && op->tail) {
	                ep->prev = op->tail ;
	                (op->tail)->next = ep ;
	                op->tail = ep ;
		    } else if ((op->head == np) && (op->tail == np)) {
	                op->head = ep ;
	                op->tail = ep ;
	                ep->prev = ep->next = nullptr ;
	            } else {
			rs = SR_BADFMT ;
	            }
	            op->n += 1 ;
		    if (rs < 0) {
			entry_finish(ep) ;
		    } /* end if (error) */
	        } /* end if (entry_start) */
		n = op->n ;
	        if (rs < 0) {
		    libmem.free(vp) ;
		} /* end if (error) */
	    } /* end if (m-a) */
	} /* end if (magic) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (fifoelem_add) */

int fifoelem_rem(FE *op,void *ebuf,int elen) noex {
	int		rs ;
	int		rs1 ;
	int		slen ; /* return-value */
	if ((rs = fifoelem_magic(op,ebuf)) >= 0) {
	    rs = SR_NOTFOUND ;
	    if (op->head) {
		FE_ENT	*ep = op->head ;
		rs = SR_TOOBIG ;
	        if ((elen < 0) || (ep->dl <= elen)) {
	            memcopy(ebuf,ep->dp,ep->dl) ;
		    slen = ep->dl ;
		    op->head = ep->next ;
		    if (op->head == nullptr) {
	    	       op->tail = nullptr ;
		    } else {
	    	        (op->head)->prev = nullptr ;
		    }
		    {
		       rs1 = entry_finish(ep) ;
		       if (rs >= 0) rs = rs1 ;
		    }
		    {
		       rs1 = libmem.free(ep) ;
		       if (rs >= 0) rs = rs1 ;
		    }
		    op->n -= 1 ;
	        } /* end if (valid) */
	    } /* end if (empty) */
	} /* end if (magic) */
	return (rs >= 0) ? slen : rs ;
}
/* end subroutine (fifoelem_rem) */

int fifoelem_get(FE *op,FE_ENT **epp) noex {
    	cnullptr	np{} ;
    	int		rs ;
	int		dl = 0 ; /* return-value */
	if ((rs = fifoelem_magic(op)) >= 0) {
	    FE_ENT	*ep = nullptr ;
	    if (op->head && op->tail) {
		ep = op->head ;
		dl = ep->dl ;
	    } else if ((op->head == np) && (op->tail == np)) {
		rs = SR_NOTFOUND ;
	    } else {
		rs = SR_BADFMT ;
	    }
	    if (epp) *epp = ep ;
	} /* end if (magic) */
	return (rs >= 0) ? dl : rs ;
}
/* end subroutine (fifoelem_get) */

int fifoelem_del(FE *op) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ; /* return-value */
	if ((rs = fifoelem_magic(op)) >= 0) {
	    if (op->head && op->tail) {
	        FE_ENT	*ep = op->head ;
	        op->head = ep->next ;
	        if (op->head == nullptr) {
	            op->tail = nullptr ;
	        } else {
	            (op->head)->prev = nullptr ;
	        }
	        {
	            rs1 = entry_finish(ep) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        {
	            rs1 = libmem.free(ep) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        op->n -= 1 ;
	        n = op->n ;
	    } else if ((op->head == np) && (op->tail == np)) {
		rs = SR_NOTFOUND ;
	    } else {
		rs = SR_BADFMT ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (fifoelem_del) */

int fifoelem_count(FE *op) noex {
    	int		rs ;
	int		n = 0 ; /* return-value */
	if ((rs = fifoelem_magic(op)) >= 0) {
	    n = op->n ;
	}
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (fifoelem_count) */

int fifoelem_curbegin(FE *op,FE_CUR *curp) noex {
    	int		rs ;
	if ((rs = fifoelem_magic(op,curp)) >= 0) {
	    curp->current = nullptr ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fifoelem_curbegin) */

int fifoelem_curend(FE *op,FE_CUR *curp) noex {
    	int		rs ;
	if ((rs = fifoelem_magic(op,curp)) >= 0) {
	    curp->current = nullptr ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fifoelem_curend) */

int fifoelem_curfetch(FE *op,FE_CUR *curp,FE_ENT **epp) noex {
    	cnullptr	np{} ;
    	int		rs ;
	int		dl = 0 ; /* return-value */
	if ((rs = fifoelem_magic(op,curp)) >= 0) {
	    FE_ENT	*ep = nullptr ;
	    if (op->head && op->tail) {
	        if (curp->current == np) {
	            ep = op->head ;
	        } else {
	            ep = (curp->current)->next ;
	        }
	        curp->current = ep ;
		dl = ep->dl ;
	    } else if ((op->head == np) && (op->tail == np)) {
		rs = SR_NOTFOUND ;
	    } else {
		rs = SR_BADFMT ;
	    }
	    if (epp) *epp = ep ;
	} /* end if (magic) */
	return (rs >= 0) ? dl : rs ;
}
/* end subroutine (fifoelem_curfetch) */

int fifoelem_curdel(FE *op,FE_CUR *curp) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ; /* return-value */
	if ((rs = fifoelem_magic(op,curp)) >= 0) {
	    FE_ENT	*ep = nullptr ;
	    if (op->head && op->tail) {
	        if (curp->current == nullptr) {
	            ep = op->head ;
	        } else {
	            ep = curp->current ;
	        }
	        if (ep->prev == nullptr) {
	            op->head = ep->next ;
	        } else {
	            (ep->prev)->next = ep->next ;
	        }
	        if (ep->next == nullptr) {
	            op->tail = ep->prev ;
	        } else {
	            (ep->next)->prev = ep->prev ;
	        }
	        curp->current = ep->prev ;
	    } else if ((op->head == np) && (op->tail == np)) {
		rs = SR_NOTFOUND ;
	    } else {
		rs = SR_BADFMT ;
	    }
	    {
	        rs1 = entry_finish(ep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = libmem.free(ep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->n -= 1 ;
	    n = op->n ;
	} /* end if (magic) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (fifoelem_curdel) */

int fifoelem_present(FE *op,cv *vsp,int vsl,fifoelem_cmp scmp) noex {
	int		rs ;
	int		rs1 ;
	int		dl = 0 ; /* return-value */
	if ((rs = fifoelem_magic(op,vsp)) >= 0) {
	    int		sl = vsl ;
	    cchar	*sp = charp(vsp) ;
	    if (vsl < 0) {
		sl = lenstr(sp) ;
	    }
	    if (scmp == nullptr) scmp = fifoelem_cmp(strcmp) ;
	    auto entcmp = [&sp,&sl,scmp] (FE_ENT *ep) {
		bool f = true ;
		f = f && (sl == ep->dl) ;
		f = f && scmp(sp,ep->dp) ;
		return f ;
	    } ; /* end lambda */
	    if (FE_CUR cur ; (rs = fifoelem_curbegin(op,&cur)) >= 0) {
	        FE_ENT	*ep ;
	        while ((rs = fifoelem_curfetch(op,&cur,&ep)) >= 0) {
		    if (entcmp(ep)) {
	                dl = ep->dl ;
	                break ;
	            }
	        } /* end while */
	        rs1 = fifoelem_curend(op,&cur) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	} /* end if (magic) */
	return (rs >= 0) ? dl : rs ;
}
/* end subroutine (fifoelem_present) */


/* privatesubroutines */

static int entry_start(FE_ENT *ep,cvoid *vsp,int vsl) noex {
	int		rs ;
	if (vsl < 0) {
	    cchar	*cp = charp(vsp) ;
	    vsl = lenstr(cp) ;
	}
	ep->prev = ep->next = nullptr ;
	cchar	*sp = charp(vsp) ;
	if (cchar *cp ; (rs = libmem.strw(sp,vsl,&cp)) >= 0) {
	    ep->dl = vsl ;
	    ep->dp = voidp(cp) ;
	}
	return rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(FE_ENT *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ep->dp) {
	    rs1 = libmem.free(ep->dp) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->dp = nullptr ;
	}
	ep->dl = 0 ;
	return rs ;
}
/* end subroutine (entry_finish) */



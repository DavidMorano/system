/* fifoitem SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* FIFO-item operations */
/* version %I% last-modified %G% */


/* revision history:

	- 2005-12-01, David A­D­ Morano
	Module was originally written.

*/

/* Copyright © 2005 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	fifoitem

	Description:
        This object manages a FIFO of variable sized entries and 
	of variable depth.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strcmp(3c)| */
#include	<usystem.h>
#include	<strwcmp.h>
#include	<localmisc.h>

#include	"fifoitem.h"

import libutil ;

/* local defines */

#define	FI		fifoitem
#define	FI_ENT		fifoitem_ent
#define	FI_CUR		fifoitem_cur


/* imported namespaces */


/* local typedefs */

typedef const void	cv ;
typedef fifoitem_ent *	entp ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int fifoitem_magic(fifoitem *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == FIFOITEM_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (fifoitem_magic) */

static int	entry_start(fifoitem_ent *,cvoid *,int) noex ;
static int	entry_finish(fifoitem_ent *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int fifoitem_start(fifoitem *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    op->head = nullptr ;
	    op->tail = nullptr ;
	    op->n = 0 ;
	    op->magic = FIFOITEM_MAGIC ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fifoitem_start) */

int fifoitem_finish(fifoitem *op) noex {
	int		rs ;
	if ((rs = fifoitem_magic(op)) >= 0) {
	    if (op->head && op->tail) {
	        while ((rs = fifoitem_del(op)) >= 0) ;
		if (rs == SR_NOENT) rs = SR_OK ;
	    } else if (op->head || op->tail) {
		rs = SR_BADFMT ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fifoitem_finish) */

int fifoitem_ins(fifoitem *op,cvoid *sp,int sl) noex {
	int		rs ;
	if ((rs = fifoitem_magic(op,sp)) >= 0) {
	    cint	esz = szof(fifoitem_ent) ;
	    if (void *vp ; (rs = uc_malloc(esz,&vp)) >= 0) {
	        fifoitem_ent	*ep = entp(vp) ;
	        if ((rs = entry_start(ep,sp,sl)) >= 0) {
		    if (op->head && op->tail) {
	                ep->prev = op->tail ;
	                (op->tail)->next = ep ;
	                op->tail = ep ;
		    } else if (op->head || op->tail) {
			rs = SR_BADFMT ;
	            } else {
	                op->head = ep ;
	                op->tail = ep ;
	                ep->prev = ep->next = nullptr ;
	            } /* end if */
	            op->n += 1 ;
	        } /* end if (entry_start) */
	        if (rs < 0) {
	            uc_free(vp) ;
	        }
	    } /* end if (m-a) */
	} /* end if (magic) */
	return (rs >= 0) ? op->n : rs ;
}
/* end subroutine (fifoitem_ins) */

int fifoitem_rem(fifoitem *op,void *vbuf,int vlen) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		dl = 0 ;
	if ((rs = fifoitem_magic(op,vbuf)) >= 0) {
	    rs = SR_NOENT ;
	    if (op->head && op->tail) {
	        fifoitem_ent	*ep = op->head ;
		rs = SR_OVERFLOW ;
	        if ((vlen < 0) || (ep->dl <= vlen)) {
		    rs = SR_OK ;
	            dl = ep->dl ;
	            memcpy(vbuf,ep->dp,ep->dl) ;
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
	                rs1 = uc_free(ep) ;
	                if (rs >= 0) rs = rs1 ;
	            }
	            op->n -= 1 ;
		} /* end if (not-overflow) */
	    } else if (op->head || op->tail) {
		rs = SR_BADFMT ;
	    } /* end if (not-empty) */
	} /* end if (magic) */
	return (rs >= 0) ? dl : rs ;
}
/* end subroutine (fifoitem_rem) */

int fifoitem_curbegin(fifoitem *op,fifoitem_cur *curp) noex {
	int		rs ;
	if ((rs = fifoitem_magic(op,curp)) >= 0) {
	    curp->current = nullptr ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fifoitem_curbegin) */

int fifoitem_curend(fifoitem *op,fifoitem_cur *curp) noex {
	int		rs ;
	if ((rs = fifoitem_magic(op,curp)) >= 0) {
	    curp->current = nullptr ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fifoitem_curend) */

int fifoitem_curfetch(fifoitem *op,fifoitem_cur *curp,fifoitem_ent **epp) noex {
	int		rs ;
	if ((rs = fifoitem_magic(op,curp)) >= 0) {
	    fifoitem_ent	*ep ;
	    if (curp->current == nullptr) {
	        ep = op->head ;
	    } else {
	        ep = (curp->current)->next ;
	    }
	    curp->current = ep ;
	    if (epp) {
	        *epp = ep ;
	    }
	    rs = (ep) ? ep->dl : SR_NOTFOUND ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fifoitem_curfetch) */

int fifoitem_curdel(fifoitem *op,fifoitem_cur *curp) noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if ((rs = fifoitem_magic(op)) >= 0) {
	    fifoitem_ent	*ep ;
	    if ((curp == nullptr) || (curp->current == nullptr)) {
	        ep = op->head ;
	    } else {
	        ep = curp->current ;
	    }
	    if (ep) {
	        if (curp) {
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
	        } else {
	            op->head = ep->next ;
	            if (op->head == nullptr) {
	                op->tail = nullptr ;
	            } else {
	                (op->head)->prev = nullptr ;
	            }
	        } /* end if */
	        {
	            rs1 = entry_finish(ep) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        {
	            rs1 = uc_free(ep) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        op->n -= 1 ;
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	    n = op->n ;
	} /* end if (magic) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (fifoitem_curdel) */

int fifoitem_count(fifoitem *op) noex {
	int		rs ;
	if ((rs = fifoitem_magic(op)) >= 0) {
	    rs = op->n ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fifoitem_count) */

int fifoitem_del(fifoitem *op) noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if ((rs = fifoitem_magic(op)) >= 0) {
	    rs = SR_NOTFOUND ;
	    if (op->head && op->tail) {
	        fifoitem_ent	*ep = op->head ;
		rs = SR_OK ;
	        op->head = ep->next ;
	        {
	            rs1 = entry_finish(ep) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        {
	            rs1 = uc_free(ep) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        op->n -= 1 ;
	    } else if (op->head || op->tail) {
		rs = SR_BADFMT ;
	    } /* end if (had entry) */
	    n = op->n ;
	} /* end if (magic) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (fifoitem_del) */

int fifoitem_present(fifoitem *op,cv *sp,int sl,fifoitem_cmp scmp) noex {
	int		rs ;
	int		rs1 ;
	int		dl = 0 ;
	if ((rs = fifoitem_magic(op,sp)) >= 0) {
	    if (sl < 0) {
		cchar	*cp = charp(sp) ;
		sl = lenstr(cp) ;
	    }
	    if (scmp == nullptr) scmp = fifoitem_cmp(strcmp) ;
	    auto entcmp = [&sp,&sl,scmp] (FI_ENT *ep) {
		bool f = true ;
		f = f && (sl == ep->dl) ;
		f = f && scmp(sp,ep->dp) ;
		return f ;
	    } ; /* end lambda */
	    if (FI_CUR cur ; (rs = fifoitem_curbegin(op,&cur)) >= 0) {
	        FI_ENT	*ep ;
	        while ((rs = fifoitem_curfetch(op,&cur,&ep)) >= 0) {
		    if (entcmp(ep)) {
	                dl = ep->dl ;
	                break ;
	            }
	        } /* end while */
	        rs1 = fifoitem_curend(op,&cur) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	} /* end if (magic) */
	return (rs >= 0) ? dl : rs ;
}
/* end subroutine (fifoitem_present) */


/* private subroutines */

static int entry_start(fifoitem_ent *ep,cvoid *vp,int sl) noex {
	int		rs ;
	cchar		*sp = ccharp(vp) ;
	if (sl < 0) sl = lenstr(sp) ;
	ep->next = nullptr ;
	ep->prev = nullptr ;
	if (cvoid *dp ; (rs = uc_mallocbuf(sp,sl,&dp)) >= 0) {
	    ep->dp = cvoidp(dp) ;
	    ep->dl = sl ;
	}
	return rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(fifoitem_ent *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ep->dp) {
	    rs1 = uc_free(ep->dp) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->dp = nullptr ;
	}
	ep->next = nullptr ;
	ep->prev = nullptr ;
	return rs ;
}
/* end subroutine (entry_finish) */



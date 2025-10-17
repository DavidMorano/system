/* setint SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* unordered set of integers */
/* version %I% last-modified %G% */


/* revision history:

	= 2013-07-11, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2013 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	setint

	Description:
	This object provides a set of integers. No two elements can
	be the same (desired for these purposes).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* |nothrow(3c++)| */
#include	<initializer_list>
#include	<utility>
#include	<functional>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<unordered_set>
#include	<usystem.h>
#include	<localmisc.h>

#include	"setint.h"


/* local defines */


/* imported namespaces */

using std::unordered_set ;		/* type-template */
using std::pair ;			/* type-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef unordered_set<int>		usetint ;
typedef unordered_set<int> *		usetintp ;
typedef unordered_set<int>::iterator	usetintit ;
typedef unordered_set<int>::iterator *	usetintitp ;


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int setint_start(setint *op) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOMEM ;
	    if (usetint *setp ; (setp = new(nothrow) usetint) != np) {
	        op->setp = voidp(setp) ;
	        rs = SR_OK ;
	    } /* end if (new-usetint) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (setint_start) */

int setint_finish(setint *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->setp) {
	        usetint		*setp = usetintp(op->setp) ;
	        delete setp ;
	        op->setp = nullptr ;
		rs = SR_OK ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (setint_finish) */

int setint_addval(setint *op,int v) noex {
	int		rs = SR_FAULT ;
	int		f = INT_MAX ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->setp) {
	        usetint		*setp = usetintp(op->setp) ;
	        pair<usetint::iterator,bool>	ret ;
	        ret = setp->insert(v) ;
	        if (ret.second == true) f = 0 ;
		rs = SR_OK ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (setint_addval) */

int setint_delval(setint *op,int v) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->setp) {
	        usetint		*setp = usetintp(op->setp) ;
	        setp->erase(v) ;
		rs = SR_OK ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (setint_delval) */

/* return the count of the number of items in this list */
int setint_count(setint *op) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->setp) {
	        usetint		*setp = usetintp(op->setp) ;
		{
		    csize	sz = setp->size() ;
	            c = intconv(sz) ;
		    rs = SR_OK ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (setint_count) */

/* return the extent of the number of items in this list */
int setint_extent(setint *op) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->setp) {
	        usetint		*setp = usetintp(op->setp) ;
		{
	            csize	sz = setp->max_size() ;
		    c = intconv(sz) ;
		    rs = SR_OK ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (setint_extent) */

int setint_mkvec(setint *op,int *va) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->setp) {
	        usetint		*setp = usetintp(op->setp) ;
		rs = SR_OK ;
	        if (va) {
		    usetintit	it = setp->begin() ;
	            usetintit	it_end = setp->end() ;
	            while (it != it_end) {
	                va[c++] = *it++ ;
	            } /* end while */
	        } else {
		    csize	sz = setp->size() ;
		    c = intconv(sz) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (setint_mkvec) */

int setint_curbegin(setint *op,setint_cur *curp) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && curp) {
	    rs = SR_NOTOPEN ;
	    if (op->setp) {
	        usetint		*setp = usetintp(op->setp) ;
	        usetintit	*interp ;
		rs = SR_NOMEM ;
	        if ((interp = new(nothrow) usetintit) != np) {
	            *interp = setp->begin() ;
	            curp->interp = voidp(interp) ;
		    rs = SR_OK ;
		} /* end if (new-usetintit) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (setint_curbegin) */

int setint_curend(setint *op,setint_cur *curp) noex {
	int		rs = SR_FAULT ;
	if (op && curp) {
	    rs = SR_NOTOPEN ;
	    if (op->setp) {
		rs = SR_BUGCHECK ;
	        if (curp->interp) {
		    usetintit	*interp = usetintitp(curp->interp) ;
	            delete interp ;
	            curp->interp = nullptr ;
		    rs = SR_OK ;
		} /* end if (cursor-valid) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (setint_curend) */

int setint_curenum(setint *op,setint_cur *curp,int *rp) noex {
	int		rs = SR_FAULT ;
	if (op && curp && rp) {
	    rs = SR_NOTOPEN ;
	    if (op->setp) {
		rs = SR_BUGCHECK ;
	        if (curp->interp) {
		    usetint	*setp = usetintp(op->setp) ;
		    usetintit	it_end = setp->end() ;
		    usetintit	*interp = usetintitp(curp->interp) ;
		    rs = SR_NOTFOUND ;
	            if (*interp != it_end) {
	                *rp = *(*interp) ;
	                (*interp)++ ;
		        rs = SR_OK ;
	            }
		} /* end if (cursor-valid) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (setint_curenum) */



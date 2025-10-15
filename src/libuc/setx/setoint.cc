/* setoint SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* ordered set of integers */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	setoint

	Description:
	This object provides am ordered set of integers.  No two
	elements can be the same (desired for these purposes).

	= Implementation

	We use the C++ STL |set| container object and let the comparison
	object default to |less|.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* |nothrow(3c++)| */
#include	<utility>
#include	<set>
#include	<usystem.h>
#include	<localmisc.h>

#include	"setoint.h"


/* local defines */


/* imported namespaces */

using std::set ;			/* type-template */
using std::pair ;			/* type-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef	set<int>		setint ;
typedef	set<int> *		setintp ;

typedef set<int>::iterator	setit ;
typedef set<int>::iterator *	setitp ;


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int setoint_start(setoint *op) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOMEM ;
	    try {
	        if (setint *setp ; (setp = new(nothrow) setint) != np) {
	            op->setp = setp ;
		    rs = SR_OK ;
	        } /* end if (new-setint) */
	    } catch (...) {
		rs = SR_NOMEM ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (setoint_start) */

int setoint_finish(setoint *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->setp) {
	        setint		*setp  = setintp(op->setp) ;
	        delete setp ;
	        op->setp = nullptr ;
		rs = SR_OK ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (setoint_finish) */

int setoint_addval(setoint *op,int v) noex {
	int		rs = SR_FAULT ;
	int		f = INT_MAX ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->setp) {
		try {
	            setint	*setp  = setintp(op->setp) ;
	            pair<setint::iterator,bool>	ret ;
	            ret = setp->insert(v) ;
	            if (ret.second == true) f = 0 ;
		    rs = SR_OK ;
		} catch (...) {
		    rs = SR_NOMEM ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (setoint_addval) */

int setoint_delval(setoint *op,int v) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->setp) {
	        setint	*setp  = setintp(op->setp) ;
	        setp->erase(v) ;
		rs = SR_OK ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (setoint_delval) */

int setoint_count(setoint *op) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->setp) {
	        setint	*setp  = setintp(op->setp) ;
		{
		    csize	sz = setp->size() ;
	            c = intconv(sz) ;
		    rs = SR_OK ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (setoint_count) */

int setoint_extent(setoint *op) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->setp) {
	        setint	*setp  = setintp(op->setp) ;
		{
		    csize	sz = setp->max_size() ;
	            c = intconv(sz) ;
		    rs = SR_OK ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (setoint_extent) */

int setoint_mkvec(setoint *op,int *va) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op && va) {
	    rs = SR_NOTOPEN ;
	    if (op->setp) {
		setint	*setp = setintp(op->setp) ;
		rs = SR_OK ;
	        if (va) {
	            setint::iterator	it = setp->begin() ;
	            setint::iterator	it_end = setp->end() ;
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
/* end subroutine (setoint_mkvec) */

int setoint_curbegin(setoint *op,setoint_cur *curp) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && curp) {
	    rs = SR_NOTOPEN ;
	    if (op->setp) {
		setint		*setp = setintp(op->setp) ;
	        setint::iterator	*interp ;
		rs = SR_NOMEM ;
	        if ((interp = new(nothrow) setint::iterator) != np) {
	            *interp = setp->begin() ;
	            curp->interp = voidp(interp) ;
		    rs = SR_OK ;
		} /* end if (new-setoint) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (setoint_curbegin) */

int setoint_curend(setoint *op,setoint_cur *curp) noex {
	int		rs = SR_FAULT ;
	if (op && curp) {
	    rs = SR_NOTOPEN ;
	    if (op->setp) {
		rs = SR_BUGCHECK ;
	        if (curp->interp) {
		    setit	*interp = setitp(curp->interp) ;
	            delete interp ;
	            curp->interp = nullptr ;
		    rs = SR_OK ;
		} /* end if (cursor-valid) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (setoint_curend) */

int setoint_curenum(setoint *op,setoint_cur *curp,int *rp) noex {
	int		rs = SR_FAULT ;
	if (op && curp && rp) {
	    rs = SR_NOTOPEN ;
	    if (op->setp) {
		rs = SR_BUGCHECK ;
	        setint		*setp  = setintp(op->setp) ;
	        if (curp->interp) {
		    setit	*interp = setitp(curp->interp) ;
	            setit	it_end = setp->end() ;
		    rs = SR_NOTFOUND ;
	            if (*interp != it_end) {
	                *rp = *(*interp) ;
	                (*interp)++ ;
			rs = SR_OK ;
	            } /* end if (found) */
		} /* end if (cursor-valid) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (setoint_curenum) */



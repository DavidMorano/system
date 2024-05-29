/* osetint SUPPORT */
/* lang=C++98 */

/* ordered set of integers */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object provides am ordered set of integers.  No two
	elements can be the same (desired for these purposes).

	= Implementation

	We use the C++ STL |set| container object and let the comparison
	object default to |less|.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<new>
#include	<utility>
#include	<set>
#include	<usystem.h>
#include	<localmisc.h>

#include	"osetint.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
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

int osetint_start(osetint *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    if (setint *setp ; (setp = new(nothrow) setint) != np) {
	        op->setp = setp ;
		rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (osetint_start) */

int osetint_finish(osetint *op) noex {
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
/* end subroutine (osetint_finish) */

int osetint_addval(osetint *op,int v) noex {
	int		rs = SR_FAULT ;
	int		f = INT_MAX ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->setp) {
	        setint		*setp  = setintp(op->setp) ;
	        pair<setint::iterator,bool>	ret ;
	        ret = setp->insert(v) ;
	        if (ret.second == true) f = 0 ;
		rs = SR_OK ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (osetint_addval) */

int osetint_delval(osetint *op,int v) noex {
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
/* end subroutine (osetint_delval) */

int osetint_count(osetint *op) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->setp) {
	        setint	*setp  = setintp(op->setp) ;
	        c = setp->size() ;
		rs = SR_OK ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (osetint_count) */

int osetint_extent(osetint *op) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->setp) {
	        setint	*setp  = setintp(op->setp) ;
	        c = setp->max_size() ;
		rs = SR_OK ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (osetint_extent) */

int osetint_mkvec(osetint *op,int *va) noex {
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
		    c = setp->size() ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (osetint_mkvec) */

int osetint_curbegin(osetint *op,osetint_cur *curp) noex {
	int		rs = SR_FAULT ;
	if (op && curp) {
	    rs = SR_NOTOPEN ;
	    if (op->setp) {
		cnullptr	np{} ;
		setint		*setp = setintp(op->setp) ;
	        setint::iterator	*interp ;
		rs = SR_NOMEM ;
	        if ((interp = new(nothrow) setint::iterator) != np) {
	            *interp = setp->begin() ;
	            curp->interp = voidp(interp) ;
		    rs = SR_OK ;
		} /* end if (new-osetint) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (osetint_curbegin) */

int osetint_curend(osetint *op,osetint_cur *curp) noex {
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
/* end subroutine (osetint_curend) */

int osetint_enum(osetint *op,osetint_cur *curp,int *rp) noex {
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
/* end subroutine (osetint_enum) */



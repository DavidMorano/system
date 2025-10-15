/* setostr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* ordered set of strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	setostr

	Description:
	This object provides am ordered set of strings.  No two
	strings can be the same (desired for these purposes).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* |nothrow(3c++) */
#include	<utility>		/* |pair(3c++)| */
#include	<string>		/* |string(3c++)| */
#include	<set>
#include	<usystem.h>
#include	<localmisc.h>

#include	"setostr.h"

import libutil ;

/* local defines */


/* imported namespaces */

using std::pair ;			/* type */
using std::set ;			/* type */
using std::string ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef set<string>		setstr ;
typedef set<string> *		setstrp ;

typedef	set<string>::iterator	iter ;
typedef	set<string>::iterator *	iterp ;


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int setostr_start(setostr *op,int n) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (n < 0) n = 0 ;
	if (op) {
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    if (setstr *setp ; (setp = new(nothrow) setstr) != np) {
	        op->setp = setp ;
		op->magic = SETOSTR_MAGIC ;
		rs = SR_OK ;
	    } /* end if (new-setsrt) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (setostr_start) */

int setostr_finish(setostr *op) noex {
	int		rs ;
	if ((rs = setostr_magic(op)) >= 0) {
	    setstr	*setp  = setstrp(op->setp) ;
	    rs = SR_BADFMT ;
	    if (setp) {
		rs = SR_OK ;
	        delete setp ;
	        op->setp = nullptr ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (setostr_finish) */

int setostr_already(setostr *op,cchar *sp,int sl) noex {
	cnullptr	np{} ;
	int		rs ;
	int		f = true ;
	if ((rs = setostr_magic(op,sp)) >= 0) {
	    setstr	*setp  = setstrp(op->setp) ;
	    if (sl < 0) sl = lenstr(sp) ;
	    if (string *strp ; (strp = new(nothrow) string(sp,sl)) != np) {
	        iter	ite = setp->end() ;
	        if (iter it ; (it = setp->find(*strp)) == ite) {
	            f = false ;
		}
		delete strp ;
	    } else {
		rs = SR_NOMEM ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (setostr_already) */

int setostr_add(setostr *op,cchar *sp,int sl) noex {
	int		rs ;
	int		f = false ;
	if ((rs = setostr_magic(op,sp)) >= 0) {
	    if (sl < 0) sl = lenstr(sp) ;
	    setstr	*setp  = setstrp(op->setp) ;
	    pair<iter,bool>	ret ;
	    string	v(sp,sl) ;
	    ret = setp->insert(v) ;
	    f = (ret.second == false) ;
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (setostr_add) */

int setostr_del(setostr *op,cchar *sp,int sl) noex {
	int		rs ;
	int		f = false ;
	if ((rs = setostr_magic(op)) >= 0) {
	    setstr	*setp  = setstrp(op->setp) ;
	    if (sl < 0) sl = lenstr(sp) ;
	    {
	        iter	ite = setp->end() ;
	        string	v(sp,sl) ;
	        if (iter it ; (it = setp->find(v)) != ite) {
		    f = true ;
		    setp->erase(it) ;
	        }
	    }
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (setostr_del) */

int setostr_delall(setostr *op) noex {
	int		rs ;
	if ((rs = setostr_magic(op)) >= 0) {
	    setstr	*setp  = setstrp(op->setp) ;
 	    {
	        iter	ite = setp->end() ;
	        if (iter it = setp->begin() ; it != ite) {
		    setp->erase(it,ite) ;
	        }
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (setostr_delall) */

int setostr_count(setostr *op) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = setostr_magic(op)) >= 0) {
	    setstr	*setp  = setstrp(op->setp) ;
	    {
	        csize	sz = setp->size() ;
	        c = intconv(sz) ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (setostr_count) */

int setostr_curbegin(setostr *op,setostr_cur *curp) noex {
	cnullptr	np{} ;
	int		rs ;
	if ((rs = setostr_magic(op,curp)) >= 0) {
	    setstr	*setp  = setstrp(op->setp) ;
	    if (iter *itp ; (itp = new(nothrow) iter) != np) {
	        *itp = setp->begin() ;
	        curp->itp = itp ;
	    } else {
		rs = SR_NOMEM ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (setostr_curbegin) */

int setostr_curend(setostr *op,setostr_cur *curp) noex {
	int		rs ;
	if ((rs = setostr_magic(op,curp)) >= 0) {
	    if (curp->itp) {
	        iter	*itp = iterp(curp->itp) ;
	        delete itp ;
	        curp->itp = nullptr ;
	    } else {
	        rs = SR_BUGCHECK ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (setostr_curend) */

int setostr_curenum(setostr *op,setostr_cur *curp,cchar **rpp) noex {
	int		rs ;
	if ((rs = setostr_magic(op,curp,rpp)) >= 0) {
	    setstr	*setp  = setstrp(op->setp) ;
	    if (curp->itp) {
	        iter	ite = setp->end() ;
	        iter	*itp = iterp(curp->itp) ;
	        if (*itp != ite) {
		    csize	sz = (*(*itp)).length() ;
		    *rpp = (*(*itp)).c_str() ;
		    rs = intconv(sz) ;
	            (*itp)++ ;
	        } else {
		    rs = SR_NOTFOUND ;
	        }
	    } else {
	        rs = SR_BUGCHECK ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (setostr_curenum) */

int setostr::already(cchar *sp,int sl) noex {
	return setostr_already(this,sp,sl) ;
}

int setostr::add(cchar *sp,int sl) noex {
	return setostr_add(this,sp,sl) ;
}

int setostr::del(cchar *sp,int sl) noex {
	return setostr_del(this,sp,sl) ;
}

void setostr::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("setostr",rs,"fini-finish") ;
	}
}

int setostr_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case setostrmem_start:
	        rs = setostr_start(op,a) ;
	        break ;
	    case setostrmem_delall:
	        rs = setostr_delall(op) ;
	        break ;
	    case setostrmem_count:
	        rs = setostr_count(op) ;
	        break ;
	    case setostrmem_finish:
	        rs = setostr_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (setostr_co::operator) */



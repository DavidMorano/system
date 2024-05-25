/* osetstr SUPPORT */
/* lang=C++98 */

/* ordered set of strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object provides am ordered set of strings.  No two
	strings can be the same (desired for these purposes).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstring>		/* |strlen(3c)| */
#include	<new>
#include	<utility>		/* |pair(3c++)| */
#include	<set>
#include	<string>
#include	<usystem.h>
#include	<localmisc.h>

#include	"osetstr.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::pair ;			/* type */
using std::set ;			/* type */
using std::string ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef set<string>	setstr ;
typedef set<string> *	setstrp ;

typedef	set<string>::iterator	iter ;
typedef	set<string>::iterator *	iterp ;


/* external subroutines */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int osetstr_start(osetstr *op,int n) noex {
	int		rs = SR_FAULT ;
	if (n < 0) n = 0 ;
	if (op) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    if (setstr *setp ; (setp = new(nothrow) setstr) != np) {
	        op->setp = setp ;
		op->magic = OSETSTR_MAGIC ;
		rs = SR_OK ;
	    } /* end if (new-setsrt) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (osetstr_start) */

int osetstr_finish(osetstr *op) noex {
	int		rs ;
	if ((rs = osetstr_magic(op)) >= 0) {
	    setstr	*setp  = setstrp(op->setp) ;
	    delete setp ;
	    op->setp = nullptr ;
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (osetstr_finish) */

int osetstr_already(osetstr *op,cchar *sp,int sl) noex {
	int		rs ;
	int		f = true ;
	if ((rs = osetstr_magic(op,sp)) >= 0) {
	    cnullptr	np{} ;
	    setstr	*setp  = setstrp(op->setp) ;
	    if (sl < 0) sl = strlen(sp) ;
	    if (string *strp ; (strp = new(nothrow) string(sp,sl)) != np) {
	        iter	ite = setp->end() ;
	        iter	it ;
	        if ((it = setp->find(*strp)) == ite) {
	            f = false ;
		}
		delete strp ;
	    } else {
		rs = SR_NOMEM ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (osetstr_already) */

int osetstr_add(osetstr *op,cchar *sp,int sl) noex {
	int		rs ;
	int		f = false ;
	if ((rs = osetstr_magic(op,sp)) >= 0) {
	    if (sl < 0) sl = strlen(sp) ;
	    setstr	*setp  = setstrp(op->setp) ;
	    pair<iter,bool>	ret ;
	    string	v(sp,sl) ;
	    ret = setp->insert(v) ;
	    f = (ret.second == false) ;
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (osetstr_add) */

int osetstr_del(osetstr *op,cchar *sp,int sl) noex {
	int		rs ;
	int		f = false ;
	if ((rs = osetstr_magic(op)) >= 0) {
	    setstr	*setp  = setstrp(op->setp) ;
	    if (sl < 0) sl = strlen(sp) ;
	    {
	        iter		it ;
	        iter		ite = setp->end() ;
	        string		v(sp,sl) ;
	        if ((it = setp->find(v)) != ite) {
		    f = true ;
		    setp->erase(it) ;
	        }
	    }
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (osetstr_del) */

int osetstr_delall(osetstr *op) noex {
	int		rs ;
	if ((rs = osetstr_magic(op)) >= 0) {
	    setstr	*setp  = setstrp(op->setp) ;
 	    {
	        iter	it = setp->begin() ;
	        iter	ite = setp->end() ;
	        if (it != ite) {
		    setp->erase(it,ite) ;
	        }
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (osetstr_delall) */

int osetstr_count(osetstr *op) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = osetstr_magic(op)) >= 0) {
	    setstr	*setp  = setstrp(op->setp) ;
	    {
	        c = setp->size() ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (osetstr_count) */

int osetstr_curbegin(osetstr *op,osetstr_cur *curp) noex {
	int		rs ;
	if ((rs = osetstr_magic(op,curp)) >= 0) {
	    cnullptr	np{} ;
	    setstr	*setp  = setstrp(op->setp) ;
	    iter	*itp ;
	    if ((itp = new(nothrow) iter) != np) {
	        *itp = setp->begin() ;
	        curp->itp = itp ;
	    } else {
		rs = SR_NOMEM ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (osetstr_curbegin) */

int osetstr_curend(osetstr *op,osetstr_cur *curp) noex {
	int		rs ;
	if ((rs = osetstr_magic(op,curp)) >= 0) {
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
/* end subroutine (osetstr_curend) */

int osetstr_enum(osetstr *op,osetstr_cur *curp,cchar **rpp) noex {
	int		rs ;
	if ((rs = osetstr_magic(op,curp,rpp)) >= 0) {
	    setstr	*setp  = setstrp(op->setp) ;
	    if (curp->itp) {
	        iter	ite = setp->end() ;
	        iter	*itp = iterp(curp->itp) ;
	        if (*itp != ite) {
		    *rpp = (*(*itp)).c_str() ;
		    rs = (*(*itp)).length() ;
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
/* end subroutine (osetstr_enum) */



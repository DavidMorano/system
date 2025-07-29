/* ureserve4 MODULE */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* reserved interfaces */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2020-05-07, David A-D- Morano
	I converted this (formerly a header-only file) to a module.

*/

/* Copyright © 1998,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Module:
	ureserve

	Description:
	This module provides some subroutines are used in certain
	circumstances where the LIBUC library is not availble (for
	whatever reasons).

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| + |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<stdexcept>		/* |std::out_of_range(3c++)| */
#include	<vector>
#include	<string>
#include	<string_view>
#include	<algorithm>		/* |ranges::sort(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ulogerror.h>
#include	<ascii.h>
#include	<localmisc.h>

module ureserve ;

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */

using std::string ;			/* type */
using std::string_view ;		/* type */
using std::ranges::sort ;		/* niebloid */
using std::ranges::binary_search ;	/* niebloid */


/* local typedefs */

typedef string_view		strview ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int vecstr::add(cchar *sp,int sl) noex {
    	int		rs = SR_FAULT ;
	if (sp) ylikely {
    	    rs = SR_NOTOPEN ;
	    if (fl.open) ylikely {
		cint	msl = lenstr(sp,sl) ;
	        try {
	            string	s(sp,msl) ;
		    push_back(s) ;
		    fl.sorted = false ;
		    {
			csize cnt = size() ;
		        rs = intconv(cnt - 1) ;
		    }
	        } catch (...) {
		    rs = SR_NOMEM ;
	        }
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
} /* end method (vecstr::add) */

int vecstr::adduniq(cchar *sp,int sl) noex {
    	int		rs ;
	if ((rs = find(sp,sl)) >= 0) {
	    rs = INT_MAX ;
	} else if (rs == SR_NOTFOUND) {
	    rs = add(sp,sl) ;
	}
	return rs ;
} /* end method (vecstr::adduniq) */

int vecstr::find(cchar *sp,int sl) noex {
    	int		rs = SR_FAULT ;
	int		c = -1 ; /* return-value */
	if (sp) ylikely {
    	    rs = SR_NOTOPEN ;
	    if (fl.open) ylikely {
		if (sl < 0) sl = lenstr(sp) ;
		rs = ifind(sp,sl) ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end method (vecstr::find) */

int vecstr::search(cchar *sp,int sl) noex {
    	cint		rsn = SR_NOTFOUND ;
    	int		rs = SR_FAULT ;
	int		f = false ;
	if (sp) ylikely {
	    rs = SR_NOTOPEN ;
	    if (fl.open) ylikely {
		if (sl < 0) sl = lenstr(sp) ;
		if (fl.sorted) {
    		    strview	s(sp,sl) ;
    		    rs = int(binary_search(*this,s)) ;
		} else {
		    if ((rs = ifind(sp,sl)) >= 0) {
			f = true ;
		    } else if (rs == rsn) {
			rs = SR_OK ;
		    }
		} /* end if (sorted or not) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
} /* end method (vecstr::search) */

int vecstr::get(int ai,ccharpp rpp) noex {
    	int		rs = SR_NOTOPEN ;
	if (fl.open) ylikely {
	    csize cnt = size() ;
	    if (ai >= 0) ylikely {
		cint	n = intconv(cnt) ;
		rs = SR_NOTFOUND ;
		if (ai < n) ylikely {
		    try {
		        string &s = at(ai) ;
	    	        rs = SR_OK ;
		        if (rpp) {
			    *rpp = s.c_str() ;
			}
		    } catch (const std::out_of_range &) {
		        rs = SR_BUGCHECK ;
		    }
		} /* end if (request in-range) */
	    }
	}
	return rs ;
} /* end method (vecstr::get) */

int vecstr::del(int ai) noex {
    	int		rs = SR_NOTOPEN ;
	if (fl.open) ylikely {
	    csize cnt = size() ;
	    rs = SR_INVALID ;
	    if (ai >= 0) ylikely {
		cint	n = intconv(cnt) ;
		rs = SR_NOTFOUND ;
		if (ai < n) ylikely {
		    iterator it = begin() ;
		    erase(it + ai) ;
		    rs = SR_OK ;
		}
	    }
	} /* end if (open) */
	return rs ;
} /* end method (vecstr::del) */

int vecstr::curbegin(vecstr_cur *curp) noex {
    	int		rs = SR_FAULT ;
	if (curp) ylikely {
	    rs = SR_NOTOPEN ;
	    if (fl.open) ylikely {
	        curp->it = begin() ;
	        rs = SR_OK ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end method (vecstr::curbegin) */

int vecstr::curend(vecstr_cur *curp) noex {
    	int		rs = SR_FAULT ;
	if (curp) ylikely {
	    rs = SR_NOTOPEN ;
	    if (fl.open) ylikely {
	        curp->it = end() ;
	        rs = SR_OK ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end method (vecstr::curend) */

int vecstr::curenum(vecstr_cur *curp,ccharpp rpp) noex {
    	int		rs = SR_FAULT ;
	if (curp && rpp) ylikely {
	    rs = SR_NOTOPEN ;
	    if (fl.open) ylikely {
	        iterator ite = end() ;
	        rs = SR_NOTFOUND ;
	        if (curp->it != ite) ylikely {
		    string	&s = *curp->it ;
		    *rpp = s.c_str() ;
		    curp->it++ ;
		    rs = intconv(s.size()) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end method (vecstr::curenum) */

int vecstr::istart(int ne) noex {
    	int		rs = SR_OK ;
	if (ne > 0) ylikely {
	    try {
	        reserve(ne) ;
		rs = SR_OK ;
	    } catch (...) {
		rs = SR_NOMEM ;
	    }
	}
	fl.open = true ;
	return rs ;
}

int vecstr::ifinish() noex {
    	int		rs = SR_NOTOPEN ;
	if (fl.open) ylikely {
	    rs = SR_OK ;
	    fl.open = false ;
	}
	return rs ;
}

int vecstr::isort() noex {
    	int		rs = SR_NOTOPEN ;
	if (fl.open) ylikely {
	    std::ranges::sort(*this) ;
	    fl.sorted = true ;
	    rs = SR_OK ;
	}
	return rs ;
} /* end method (vecstr::isort) */

int vecstr::ifind(cchar *sp,int sl) noex {
    	int		rs = SR_OK ;
	int		idx = 0 ; /* return-value */
	try {
	    string	s(sp,sl) ;
	    bool	f = false ;
	    for (cauto &e : (*this)) {
		f = (e == s) ;
		if (f) break ;
		idx += 1 ;
	    } /* end for */
	    if (! f) rs = SR_NOTFOUND ;
	} catch (...) {
	    rs = SR_NOMEM ;
	}
	return (rs >= 0) ? idx : rs ;
} /* end method (vecstr::ifind) */

int vecstr::idelall() noex {
    	int		rs = SR_NOTOPEN ;
	if (fl.open) ylikely {
	    resize(0) ;
	    rs = SR_OK ;
	}
	return rs ;
} /* end method (vecstr::idelall) */

int vecstr::icount() const noex {
    	int		rs = SR_NOTOPEN ;
	if (fl.open) ylikely {
	    csize cnt = size() ;
	    rs = intconv(cnt) ;
	}
	return rs ;
} /* end method (vecstr::icount) */

void vecstr::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("vecstr",rs,"fini-finish") ;
	}
} /* end method (vecstr::dtor) */

vecstr::operator int () noex {
    	int		rs = SR_NOTOPEN ;
	if (fl.open) ylikely {
	    csize	c = size() ;
	    rs = intconv(c) ;
	}
	return rs ;
} /* end method (vecstr::operator) */

int vecstr_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case vecstrmem_start:
	        rs = op->istart(a) ;
	        break ;
	    case vecstrmem_count:
		rs = op->icount() ;
	        break ;
	    case vecstrmem_sort:
	        rs = op->isort() ;
	        break ;
	    case vecstrmem_delall:
	        rs = op->idelall() ;
	        break ;
	    case vecstrmem_finish:
	        rs = op->ifinish() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (vecstr_co::operator) */



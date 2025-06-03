/* ureserve4 MODULE */
/* encoding=ISO8859-1 */
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

  	Family:
	ureserve

	Description:
	This module provides some subroutines are used in certain
	circumstances where the LIBUC library is not availble (for
	whatever reasons).

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<vector>
#include	<string>
#include	<iostream>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ulogerror.h>
#include	<ascii.h>
#include	<localmisc.h>

module ureserve ;

import libutil ;			/* |xstrlen(3u)| */

/* local defines */


/* imported namespaces */

using std::string ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int vecstr::add(cchar *sp,int sl) noex {
    	int		rs = SR_FAULT ;
	if (sp) {
    	    rs = SR_NOTOPEN ;
	    if (fl.open) {
		cint	msl = xstrnlen(sp,sl) ;
	        try {
	            string	s(sp,msl) ;
		    push_back(s) ;
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
}

int vecstr::adduniq(cchar *sp,int sl) noex {
    	int		rs ;
	if ((rs = find(sp,sl)) >= 0) {
	    rs = INT_MAX ;
	} else if (rs == SR_NOTFOUND) {
	    rs = add(sp,sl) ;
	}
	return rs ;
}

int vecstr::find(cchar *sp,int sl) noex {
    	int		rs = SR_FAULT ;
	int		c = -1 ;
	if (sp) {
    	    rs = SR_NOTOPEN ;
	    if (fl.open) {
		bool	f = false ;
		if (sl < 0) sl = xstrlen(sp) ;
	        try {
	            string	s(sp,sl) ;
		    for (cauto &e : (*this)) {
			f = (e == s) ;
			if (f) break ;
			c += 1 ;
		    } /* end for */
	        } catch (...) {
		    rs = SR_NOMEM ;
	        }
		if (! f) rs = SR_NOTFOUND ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end method (vecstr::find) */

int vecstr::del(int ai) noex {
    	int		rs = SR_NOTOPEN ;
	if (fl.open) {
	    csize cnt = size() ;
	    rs = SR_INVALID ;
	    if (ai >= 0) {
		cint	n = intconv(cnt) ;
		if (ai < n) {
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
	if (curp) {
	    rs = SR_NOTOPEN ;
	    if (fl.open) {
	        curp->it = begin() ;
	        rs = SR_OK ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end method (vecstr::curbegin) */

int vecstr::curend(vecstr_cur *curp) noex {
    	int		rs = SR_FAULT ;
	if (curp) {
	    rs = SR_NOTOPEN ;
	    if (fl.open) {
	        curp->it = end() ;
	        rs = SR_OK ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end method (vecstr::curend) */

int vecstr::curenum(vecstr_cur *curp,ccharpp rpp) noex {
    	int		rs = SR_FAULT ;
	if (curp && rpp) {
	    rs = SR_NOTOPEN ;
	    if (fl.open) {
	        iterator ite = end() ;
	        rs = SR_NOTFOUND ;
	        if (curp->it != ite) {
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
	if (ne > 0) {
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
	if (fl.open) {
	    rs = SR_OK ;
	    fl.open = false ;
	}
	return rs ;
}

int vecstr::idelall() noex {
    	int		rs = SR_NOTOPEN ;
	if (fl.open) {
	    resize(0) ;
	    rs = SR_OK ;
	}
	return rs ;
}

void vecstr::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("vecstr",rs,"fini-finish") ;
	}
}

vecstr::operator int () noex {
    	int		rs = SR_NOTOPEN ;
	if (fl.open) {
	    csize	c = size() ;
	    rs = intconv(c) ;
	}
	return rs ;
}

int vecstr_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case vecstrmem_start:
	        rs = op->istart(a) ;
	        break ;
	    case vecstrmem_count:
		{
		    csize c = op->size() ;
		    rs = intconv(c) ;
		}
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
}
/* end method (vecstr_co::operator) */



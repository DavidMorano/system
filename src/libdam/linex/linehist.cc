/* linehist SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Line History (object) */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This was really made from scratch.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	linehist

	Names:
	linehist_start
	linehist_proc
	linehist_count
	linehist_get
	linehist_finish

	Description:
	This object both processes lines and remembers any lines
	that meet a certain criteria.  I Process characters (a line
	at a time) for balanced pairs.  I record line numbers so
	that when we are left with some sort of unbalance, we can
	report the associated line numbers.

	Dependencies:
	langstate

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<vector>
#include	<new>			/* |nothrow(3c++)| */
#include	<usystem.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"linehist.h"


/* local defines */


/* imported namespaces */

using std::vector ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    class item {
	int		ln ;		/* line number */
	int		it ;		/* 0=opening, 1=closing */
    public:
	item(int aln,int ait) noex : ln(aln), it(ait) { } ;
	int type() const { 
	    return it ; 
	} ;
	int line() const { 
	    return ln ; 
	} ;
    } ; /* end struct (item) */
} /* end namespace */

typedef vector<item>	ivec ;
typedef vector<item> *	ivecp ;


/* forward references */

template<typename ... Args>
local int linehist_ctor(linehist *op,Args ... args) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    op->lvp = nullptr ;
	    if ((op->lsp = new(nothrow) langstate) != np) {
		rs = SR_OK ;
	    } /* end if (new-langstate) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (linehist_ctor) */

local int linehist_dtor(linehist *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->lsp) {
		delete op->lsp ;
		op->lsp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (linehist_dtor) */

template<typename ... Args>
local inline int linehist_magic(linehist *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == LINEHIST_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (linehist_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int linehist_start(linehist *op,cchar *ss) noex {
	cnullptr	np{} ;
	int		rs ;
	if ((rs = linehist_ctor(op,ss)) >= 0) {
	    rs = SR_INVALID ;
	    if (ss[0]) {
		rs = SR_NOMEM ;
	        if (ivec *lvp ; (lvp = new(nothrow) ivec) != np) {
	            op->lvp = lvp ;
	            if ((rs = langstate_start(op->lsp)) >= 0) {
	                strncpy(op->ss,ss,2) ;
	                op->magic = LINEHIST_MAGIC ;
	            } /* end if (langstate_start) */
	            if (rs < 0) {
		        delete lvp ;
		        op->lvp = nullptr ;
	            } /* end if (error) */
	        } /* end if (new-ivec) */
	    } /* end if (valid) */
	    if (rs < 0) {
		linehist_dtor(op) ;
	    }
	} /* end if (linehist_ctor) */
	return rs ;
}
/* end subroutine (linehist_start) */

int linehist_finish(linehist *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = linehist_magic(op)) >= 0) {
	    if (op->lvp) {
	        ivec *lvp = ivecp(op->lvp) ;
	        delete lvp ;
	        op->lvp = nullptr ;
	    }
	    if (op->lsp) {
	        rs1 = langstate_finish(op->lsp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = linehist_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (linehist_finish) */

int linehist_proc(linehist *op,int ln,cchar *sp,int sl) noex {
	cnullptr	np{} ;
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = linehist_magic(op,sp)) >= 0) {
	    if (ivec *lvp ; (lvp = ivecp(op->lvp)) != np) {
	        cint	sch0 = mkchar(op->ss[0]) ;
	        cint	sch1 = mkchar(op->ss[1]) ;
	        while ((rs >= 0) && sl && *sp) {
		    cint	ch = mkchar(*sp) ;
		    if ((rs = langstate_proc(op->lsp,ln,ch)) > 0) {
			try {
		            if (ch == sch0) {
		                item	a(ln,0) ;
		                lvp->push_back(a) ;
		            } else if (ch == sch1) {
		                bool	f = true ;
		                if (lvp->size() > 0) {
			            const item	li = lvp->back() ;
			            if (li.type() == 0) {
			                f = false ;
			                lvp->pop_back() ;
			            }
		                }
		                if (f) {
		                    item	a(ln,1) ;
		                    lvp->push_back(a) ;
		                }
		            } /* end if */
			} catch (...) {
			    rs = SR_NOMEM ;
			}
		    } /* end if (langstate_proc) */
		    sp += 1 ;
		    sl -= 1 ;
	        } /* end while */
	        c = intconv(lvp->size()) ;
	    } else {
	        rs = SR_BUGCHECK ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (linehist_proc) */

int linehist_count(linehist *op) noex {
	cnullptr	np{} ;
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = linehist_magic(op)) >= 0) {
	    if (ivec *lvp ; (lvp = ivecp(op->lvp)) != np) {
	        c = intconv(lvp->size()) ;
	    } else {
	        rs = SR_BUGCHECK ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (linehist_count) */

int linehist_get(linehist *op,int i,int *lnp) noex {
	cnullptr	np{} ;
	int		rs ;
	int		type = 0 ; /* return-value */
	if ((rs = linehist_magic(op,lnp)) >= 0) {
	    rs = SR_INVALID ;
	    if (i >= 0) {
	        if (ivec *lvp ; (lvp = ivecp(op->lvp)) != np) {
	            cint	len = intconv(lvp->size()) ;
	            if (i < len) {
	                item	vi = lvp->at(i) ;
		        type = (vi.type() + 1) ;
		        if (lnp != nullptr) {
		            *lnp = vi.line() ;
		        }
	            }
	        } else {
	            rs = SR_BUGCHECK ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? type : rs ;
}
/* end subroutine (linehist_get) */



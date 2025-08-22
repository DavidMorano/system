/* langproc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* computer-language text processor */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This was really made from scratch.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	langproc

	Names:
	langproc_start
	langproc_proc
	langproc_count
	langproc_curbegin
	langproc_curend
	langproc_curenum
	langproc_finish

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
#include	<new>			/* |nothrow(3c++)| */
#include	<vector>
#include	<string>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"langproc.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ureserve.ccm"

import libutil ;			/* |resumelife(3u)| */
import ureserve ;			/* |vecstr(3u)| */

/* local defines */


/* imported namespaces */

using std::string ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int langproc_ctor(langproc *op,Args ... args) noex {
    	cnothrow	nt{} ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    op->lvp = nullptr ;
	    if (langstate *lsp ; (lsp = new(nt) langstate) != np) {
		op->lsp = lsp ;
		if (vecstr *lvp ; (lvp = new(nt) vecstr) != np) {
		    op->lvp = lvp ;
		    rs = SR_OK ;
		}
		if (rs < 0) {
		    delete lsp ;
		    op->lsp = nullptr ;
		}
	    } /* end if (new-langstate) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (langproc_ctor) */

local int langproc_dtor(langproc *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->lvp) {
		vecstr *lvp = resumelife<vecstr>(op->lvp) ;
		delete lvp ;
		op->lvp = nullptr ;
	    }
	    if (op->lsp) {
		delete op->lsp ;
		op->lsp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (langproc_dtor) */

template<typename ... Args>
local inline int langproc_magic(langproc *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == LANGPROC_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (langproc_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int langproc_start(langproc *op) noex {
	int		rs ;
	if ((rs = langproc_ctor(op)) >= 0) {
	    if ((rs = langstate_start(op->lsp)) >= 0) {
		vecstr *lvp = resumelife<vecstr>(op->lvp) ;
		if ((rs = lvp->start) >= 0) {
		    op->magic = LANGPROC_MAGIC ;
		}
		if (rs < 0) {
		    langstate_finish(op->lsp) ;
		}
	    } /* end if (langstate_start) */
	    if (rs < 0) {
		langproc_dtor(op) ;
	    }
	} /* end if (langproc_ctor) */
	return rs ;
}
/* end subroutine (langproc_start) */

int langproc_finish(langproc *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = langproc_magic(op)) >= 0) {
	    if (op->lvp) {
		vecstr *lvp = resumelife<vecstr>(op->lvp) ;
	        delete lvp ;
	        op->lvp = nullptr ;
	    }
	    if (op->lsp) {
	        rs1 = langstate_finish(op->lsp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = langproc_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (langproc_finish) */

#ifdef	COMMENT
int langproc_proc(langproc *op,int ln,cchar *sp,int sl) noex {
	cnullptr	np{} ;
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = langproc_magic(op,sp)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (vecstr *lvp ; (lvp = resumelife<vecstr>(op->lvp)) != np) {
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
	    } /* end if (vecstr) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (langproc_proc) */
#endif /* COMMENT */

int langproc_count(langproc *op) noex {
	cnullptr	np{} ;
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = langproc_magic(op)) >= 0) {
	    if (vecstr *lvp ; (lvp = resumelife<vecstr>(op->lvp)) != np) {
	        rs = lvp->count ;
		c = rs ;
	    } else {
	        rs = SR_BUGCHECK ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (langproc_count) */

#ifdef	COMMENT
int langproc_get(langproc *op,int i,int *lnp) noex {
	cnullptr	np{} ;
	int		rs ;
	int		type = 0 ; /* return-value */
	if ((rs = langproc_magic(op,lnp)) >= 0) {
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
/* end subroutine (langproc_get) */
#endif /* COMMENT */



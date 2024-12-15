/* langstate SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++11 */

/* Language (parse) State (object) */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-06-29, David A­D­ Morano
	This was really made from scratch.

	= 2016-06-29, David A­D­ Morano
	I updated this to add the C++11 'noexcept' where appropriate.

	= 2017-10-19, David A­D­ Morano
	Enhanced to ignore characters within character literals.

*/

/* Copyright © 2008,2016,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We track the parse state of C-language type input.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<new>
#include	<usystem.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"langstate.h"


/* local defines */


/* imported namespaces */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    class item {
	int		ln ;		/* line number */
	int		it ;		/* 0=opening, 1=closing */
    public:
	item(int aln,int ait) noex : ln(aln), it(ait) { } ;
	int type() const noex { 
	    return it ; 
	} ;
	int line() const noex { 
	    return ln ; 
	} ;
    } ; /* end struct (item) */
}


/* forward references */

template<typename ... Args>
static int langstate_ctor(langstate *op,Args ... args) noex {
    	LANGSTATE	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_OK ;
	    memclear(hop) ; /* dangerous */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (langstate_ctor) */

static int langstate_dtor(langstate *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (langstate_dtor) */

template<typename ... Args>
static int langstate_magic(langstate *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == LANGSTATE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (langstate_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int langstate_start(langstate *op) noex {
	int		rs ;
	if ((rs = langstate_ctor(op)) >= 0) {
	    op->f.clear = true ;
	    op->magic = LANGSTATE_MAGIC ;
	    if (rs < 0) {
		langstate_dtor(op) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (langstate_start) */

int langstate_finish(langstate *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = langstate_magic(op)) >= 0) {
	    {
		rs1 = langstate_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (langstate_finish) */

int langstate_proc(langstate *op,int ln,int ch) noex {
	int		rs ;
	int		f = false ;
	if ((rs = langstate_magic(op)) >= 0) {
	    f = op->f.clear ;
	    if (op->f.comment) {
	        if ((op->pch == '*') && (ch == '/')) {
		    op->f.comment = false ;
		    op->f.clear = true ;
		    op->line = 0 ;
	        }
	    } else if (op->f.quote) {
	        if (op->f.skip) {
		    op->f.skip = false ;
	        } else {
		    if (ch == CH_BSLASH) {
		        op->f.skip = true ;
	            } else if (ch == CH_DQUOTE) {
		        op->f.quote = false ;
		        op->f.clear = true ;
		        op->line = 0 ;
		    }
	        }
	    } else if (op->f.literal) {
	        if (op->f.skip) {
		    op->f.skip = false ;
	        } else {
		    if (ch == CH_BSLASH) {
		        op->f.skip = true ;
	            } else if (ch == CH_SQUOTE) {
		        op->f.literal = false ;
		        op->f.clear = true ;
		        op->line = 0 ;
		    }
	        }
	    } else {
	        switch (ch) {
	        case '*':
		    if (op->pch == '/') {
		        op->f.comment = true ;
		        op->f.clear = false ;
		        op->line = ln ;
		        f = false ;
		    }
		    break ;
	        case CH_DQUOTE:
		    if ((op->pch != CH_BSLASH) && (op->pch != CH_SQUOTE)) {
		        op->f.quote = true ;
		        op->f.clear = false ;
		        op->line = ln ;
		        f = false ;
		    }
		    break ;
	        case CH_SQUOTE:
		    if (op->pch != CH_BSLASH) {
		        op->f.literal = true ;
		        op->f.clear = false ;
		        op->line = ln ;
		        f = false ;
		    }
		    break ;
	        } /* end switch */
	    } /* end if */
	    op->pch = ch ;
	} /* end if (langstate_magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (langstate_proc) */

int langstate_stat(langstate *op,langstate_info *sbp) noex {
	int		rs ;
	int		type = langstatetype_clear ;
	if ((rs = langstate_magic(op,sbp)) >= 0) {
	    memclear(sbp) ; /* dangerous */
	    sbp->line = op->line ;
	    if (op->f.comment) {
	        type = langstatetype_comment ;
	    } else if (op->f.quote) {
	        type = langstatetype_quote ;
	    } else if (op->f.literal) {
	        type = langstatetype_literal ;
	    }
	    sbp->type = type ;
	} /* end if (magic) */
	return (rs >= 0) ? type : rs ;
}
/* end subroutine (langstate_stat) */



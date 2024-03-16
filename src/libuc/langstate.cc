/* langstate SUPPORT */
/* lang=C++98 */

/* Language (parse) State (object) */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This was really made from scratch.

	= 2017-10-19, David A­D­ Morano
	Enhanced to ignore characters within character literals.

*/

/* Copyright © 2016,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We track the parse state of C-language type input.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<climits>
#include	<cstring>
#include	<new>
#include	<usystem.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"langstate.h"


/* local defines */


/* default name spaces */

using namespace		std ;		/* yes, we want punishment! */


/* external subroutines */


/* external variables */


/* local structures */

class item {
	int		ln ;		/* line number */
	int		it ;		/* 0=opening, 1=closing */
public:
	item(int aln,int ait) : ln(aln), it(ait) { } ;
	int type() const { return it ; } ;
	int line() const { return ln ; } ;
} ;


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int langstate_start(langstate *op) noex {
	if (op == nullptr) return SR_FAULT ;
	memclear(op) ;			/* dangerous */
	op->f.clear = TRUE ;
	op->magic = LANGSTATE_MAGIC ;
	return SR_OK ;
}
/* end subroutine (langstate_start) */

int langstate_finish(langstate *op) noex {
	if (op == nullptr) return SR_FAULT ;
	if (op->magic != LANGSTATE_MAGIC) return SR_NOTOPEN ;
	op->magic = 0 ;
	return SR_OK ;
}
/* end subroutine (langstate_finish) */

int langstate_proc(langstate *op,int ln,int ch) noex {
	int		f ;

	if (op == nullptr) return SR_FAULT ;
	if (op->magic != LANGSTATE_MAGIC) return SR_NOTOPEN ;

	f = op->f.clear ;
	if (op->f.comment) {
	    if ((op->pch == '*') && (ch == '/')) {
		op->f.comment = FALSE ;
		op->f.clear = TRUE ;
		op->line = 0 ;
	    }
	} else if (op->f.quote) {
	    if (op->f.skip) {
		op->f.skip = FALSE ;
	    } else {
		if (ch == CH_BSLASH) {
		    op->f.skip = TRUE ;
	        } else if (ch == CH_DQUOTE) {
		    op->f.quote = FALSE ;
		    op->f.clear = TRUE ;
		    op->line = 0 ;
		}
	    }
	} else if (op->f.literal) {
	    if (op->f.skip) {
		op->f.skip = FALSE ;
	    } else {
		if (ch == CH_BSLASH) {
		    op->f.skip = TRUE ;
	        } else if (ch == CH_SQUOTE) {
		    op->f.literal = FALSE ;
		    op->f.clear = TRUE ;
		    op->line = 0 ;
		}
	    }
	} else {
	    switch (ch) {
	    case '*':
		if (op->pch == '/') {
		    op->f.comment = TRUE ;
		    op->f.clear = FALSE ;
		    op->line = ln ;
		    f = FALSE ;
		}
		break ;
	    case CH_DQUOTE:
		if ((op->pch != CH_BSLASH) && (op->pch != CH_SQUOTE)) {
		    op->f.quote = TRUE ;
		    op->f.clear = FALSE ;
		    op->line = ln ;
		    f = FALSE ;
		}
		break ;
	    case CH_SQUOTE:
		if (op->pch != CH_BSLASH) {
		    op->f.literal = TRUE ;
		    op->f.clear = FALSE ;
		    op->line = ln ;
		    f = FALSE ;
		}
		break ;
	    } /* end switch */
	} /* end if */
	op->pch = ch ;

	return f ;
}
/* end subroutine (langstate_proc) */

int langstate_stat(langstate *op,langstate_info *sbp) noex {
	int		rs = SR_OK ;
	int		type = langstatetype_clear ;
	if (op == nullptr) return SR_FAULT ;
	if (sbp == nullptr) return SR_FAULT ;
	memclear(sbp) ;			/* dangerous */
	sbp->line = op->line ;
	if (op->f.comment) {
	    type = langstatetype_comment ;
	} else if (op->f.quote) {
	    type = langstatetype_quote ;
	} else if (op->f.literal) {
	    type = langstatetype_literal ;
	}
	sbp->type = type ;
	return (rs >= 0) ? type : rs ;
}
/* end subroutine (langstate_stat) */



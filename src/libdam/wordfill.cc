/* wordfill HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* text (word) fill */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-03-04, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	wordfill

	Description:
	This object mediates filling out text lines for a specified
	line length.

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<estrings.h>
#include	<fifostr.h>
#include	<strn.h>
#include	<sfx.h>
#include	<localmisc.h>

#include	"wordfill.h"

import libutil ;
import sif ;

/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int wordfill_ctor(wordfill *op,Args ... args) noex {
    	WORDFILL	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->sqp = new(nothrow) fifostr) != np) {
		rs = SR_OK ;
	    } /* end if (new-fifostr) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (wordfill_ctor) */

static int wordfill_dtor(wordfill *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->sqp) {
		delete op->sqp ;
		op->sqp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (wordfill_dtor) */

template<typename ... Args>
static int wordfill_magic(wordfill *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == WORDFILL_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (wordfill_magic) */

static int	wordfill_mkline(wordfill *,int,char *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int wordfill_start(wordfill *op,cchar *lp,int ll) noex {
	int		rs ;
	if ((rs = wordfill_ctor(op)) >= 0) {
	    if ((rs = fifostr_start(op->sqp)) >= 0) {
	        op->magic = WORDFILL_MAGIC ;
	        if (lp) {
	            rs = wordfill_addlines(op,lp,ll) ;
	        }
	        if (rs < 0) {
		    fifostr_finish(op->sqp) ;
	            op->magic = 0 ;
	        }
	    } /* end if (fifostr-start) */
	    if (rs < 0) {
		wordfill_dtor(op) ;
	    }
	} /* end if (wordfill_ctor) */
	return rs ;
}
/* end subroutine (wordfill_start) */

int wordfill_finish(wordfill *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = wordfill_magic(op)) >= 0) {
	    if (op->sqp) {
	        rs1 = fifostr_finish(op->sqp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = wordfill_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (wordfill_finish) */

int wordfill_addword(wordfill *op,cchar *lbuf,int llen) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = wordfill_magic(op,lbuf)) >= 0) {
	    if (llen < 0) llen = lenstr(lbuf) ;
	    if (llen > 0) {
	        c += 1 ;
	        if ((rs = fifostr_add(op->sqp,lbuf,llen)) >= 0) {
	            op->wc += 1 ;
	            op->chrc += llen ;
	        }
	    }
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (wordfill_addword) */

int wordfill_addline(wordfill *op,cchar *lbuf,int llen) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = wordfill_magic(op,lbuf)) >= 0) {
	    if (llen < 0) llen = lenstr(lbuf) ;
	    if (llen > 0) {
		sif	so(lbuf,llen) ;
		cchar	*cp ;
		for (int cl ; (cl = so(&cp)) > 0 ; ) {
	            c += 1 ;
	            rs = fifostr_add(op->sqp,cp,cl) ;
	            op->wc += 1 ;
	            op->chrc += cl ;
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if (non-zero positive) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (wordfill_addline) */

int wordfill_addlines(wordfill *op,cchar *lbuf,int llen) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = wordfill_magic(op,lbuf)) >= 0) {
	    int		ll = (llen < 0) ? int(lenstr(lbuf)) : llen ;
	    cchar	*lp = lbuf ;
	    for (cchar *tp ; (tp = strnchr(lp,ll,'\n')) != nullptr ; ) {
		cchar	*sp ;
		cint tl = intconv(tp - lp) ;
		if (int sl ; (sl = sfcontent(lp,tl,&sp)) > 0) {
		    sif		so(sp,sl) ;
		    cchar	*cp ;
		    for (int cl ; (cl = so(&cp)) > 0 ; ) {
	                c += 1 ;
	                rs = fifostr_add(op->sqp,cp,cl) ;
		        op->wc += 1 ;
		        op->chrc += cl ;
			if (rs < 0) break ;
	            } /* end for */
		} /* end if (sfcontent) */
		ll -= intconv((tp + 1) - lp) ;
		lp = (tp + 1) ;
	        if (rs < 0) break ;
	    } /* end while */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (wordfill_addlines) */

int wordfill_mklinefull(wordfill *op,char *lbuf,int llen) noex {
	return wordfill_mkline(op,false,lbuf,llen) ;
}
/* end subroutine (wordfill_mklinefull) */

int wordfill_mklinepart(wordfill *op,char *lbuf,int llen) noex {
	return wordfill_mkline(op,true,lbuf,llen) ;
}
/* end subroutine (wordfill_mklinepart) */


/* private subroutines */

static int wordfill_mkline(wordfill *op,int f_part,char *lbuf,int llen) noex {
	int		rs ;
	int		rs1 ;
	int		tlen = 0 ;
	if ((rs = wordfill_magic(op,lbuf)) >= 0) {
	    rs = SR_INVALID ;
	    if (llen >= 1) {
	        int		ll ;
	        int		wl, nl ;
	        int		ql ;
	        int		c ;
	        int		f_give = f_part ;
	        char		*lp ;		/* pointer to write to */
		rs = SR_OK ;
	        if (! f_part) {
	            ql = op->chrc ;
	            if (op->wc > 0) {
		        ql += (op->wc - 1) ;
		    }
	            if (ql >= (llen - 1)) {
	                f_give = true ;
		    }
	        } /* end if (not requested partial) */
	        if (f_give) {
	            c = 0 ;
	            lp = lbuf ;
	            ll = llen ;
	            while (ll > 0) {
	                rs1 = fifostr_headlen(op->sqp) ;
	                wl = rs1 ;
	                if (rs1 == SR_NOTFOUND) break ;
			/* ignore zero-length words */
	                if (wl == 0) continue ;
			/* calculate needed-length ('nl') for this word */
	                nl = (c > 0) ? (wl + 1) : wl ;
			/* can this word fit in the current line? */
	                if (nl > ll) break ;
			/* yes: so remove the word from the FIFO to the line */
	                if (c++ > 0) {
	                    *lp++ = ' ' ;
	                    ll -= 1 ;
	                }
	                if ((rs = fifostr_rem(op->sqp,lp,ll)) >= 0) {
		            op->wc -= 1 ;
		            op->chrc -= wl ;
		        }
	                lp += wl ;
	                ll -= wl ;
	                if (rs < 0) break ;
	            } /* end while (getting words) */
	            tlen = (llen - ll) ;
	        } /* end if (giving) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (wordfill_mkline) */



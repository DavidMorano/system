/* bufslide_prime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++98 */

/* Output Buffer (object) */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This was carved out of an existing subroutine.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	bufslide

	Description:
	This object facilitates output buffering, but with just a 
	dynamically sized buffer -- no actual output of any sort.

	Names:
	-ctor-		
	operator-int	
	+=

	Synopses:
	int bufslide::add(cchar *sp,int sl = -1) noex
	int bufslide::adv(int al) noex
	int bufslide::len() noex
	int bufslide::operator-int

	Returns:
	>=0		current additional legnth of buffer (beyonf the start)
	<0		error (system-return) 

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<ulogerror.h>
#include	<binchunk.hh>
#include	<localmisc.h>

#include	"bufslide.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */

typedef int8_t *	int8p ;
typedef const int8_t *	cint8p ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int bufslide::istart() noex {
    	cnothrow	nt{} ;
    	int		rs = SR_INVALID ;
	if (chunkp == nullptr) ylikely {
	    oi = 0 ;
	    rs = SR_NOMEM ;
	    if (binchunk *bp = new(nt) binchunk ; bp) ylikely {
		chunkp = bp ;
		if ((rs = bp->start) >= 0) ylikely {
    	            fl.open = true ;
		}
		if (rs < 0) {
		    delete bp ;
		    bp = nullptr ;
		    chunkp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-binchunk) */
	} /* end if (non-null) */
    	return rs ;
} /* end method (bufslide::istart) */

int bufslide::ifinish() noex {
    	int		rs = SR_NOTOPEN ;
	int		rs1 ;
	if (fl.open) ylikely {
	    rs = SR_BUGCHECK ;
	    if (binchunk *bp = resumelife<binchunk>(chunkp) ; bp) ylikely {
		rs = SR_OK ;
		{
    	            rs1 = bp->finish ;
		    if (rs >= 0) rs = rs1 ;
		}
		{
		    delete bp ;
		    bp = nullptr ;
		    chunkp = nullptr ;
		}
	    } /* end if (chunkp) */
	    oi = 0 ;
    	    fl.open = false ;
	} /* end if (open) */
    	return rs ;
} /* end method (bufslide::ifinish) */

int bufslide::push(int ch) noex {
    	int		rs = SR_NOTOPEN ;
	if (fl.open) ylikely {
	    rs = SR_BUGCHECK ;
	    if (binchunk *bp = resumelife<binchunk>(chunkp) ; bp) ylikely {
	        rs = bp->add(ch) ;
	    }
	} /* end if (open) */
	return rs ;
} /* end method (bufslide::push) */

int bufslide::add(int ch) noex {
    	return push(ch) ;
} /* end method (bufslide::add) */

int bufslide::add(cchar *sp,int µsl) noex {
	int		rs = SR_FAULT ;
	if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) ylikely {
	    rs = SR_NOTOPEN ;
	    if (fl.open) ylikely {
	        rs = SR_BUGCHECK ;
	        if (binchunk *bp = resumelife<binchunk>(chunkp) ; bp) ylikely {
		    const int8_t *sb = int8p(sp) ;
	            rs = bp->add(sb,sl) ;
	        }
	    } /* end if (open) */
	} /* end if (getlenstr) */
	return rs ;
} /* end method (bufslide::add) */

int bufslide::at(int ei) const noex {
    	int		rs = SR_NOTOPEN ;
	int		rch = 0 ; /* return-value */
	if (fl.open) ylikely {
	    rs = SR_BUGCHECK ;
	    if (binchunk *bp = resumelife<binchunk>(chunkp) ; bp) ylikely {
	        if ((rs = bp->len) >= 0) {
	            cint ql = rs ;
		    rs = SR_BUGCHECK ;
		    if (oi <= ql) ylikely {
		        cint n = (ql - oi) ;
		        rs = SR_INVALID ;
		        if ((ei >= 0) && (ei < n)) ylikely {
		            rs = bp->at(oi + ei) ;
			    rch = rs ;
	                } /* end if (valid) */
		    } /* end if (valid) */
	        } /* end if (chunk_len) */
	    } /* end if (valid) */
	} /* end if (open) */
	return (rs >= 0) ? rch : rs ;
} /* end method (bufslide::at) */

int bufslide::readat(int ei,char *rbuf,int rlen) const noex {
    	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (rbuf) ylikely {
    	    int8_t *rb = int8p(rbuf) ;
	    if ((rs = ilen()) > 0) ylikely {
		if (cint cnt = rs ; (ei >= 0) && (ei < cnt)) ylikely {
	            if (binchunk *bp = resumelife<binchunk>(chunkp) ; bp) {
			rs = bp->readat((oi + ei),rb,rlen) ;
			rl = rs ;
		    } /* end if (valid) */
		} else {
		    rs = SR_INVALID ;
		} /* end if */
	    } /* end if (ilen) */
	    rbuf[rl] = '\0' ;
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
} /* end method (bufslide::readat) */

int bufslide::read(char *rbuf,int rlen) const noex {
    	return readat(0,rbuf,rlen) ;
} /* end method (bufslide::read) */

/* return the number advanced or error */
int bufslide::iadv(int n) noex {
    	int		rs = SR_NOTOPEN ;
	int		al = 0 ; /* return-value */
	if (fl.open) ylikely {
	    rs = SR_BUGCHECK ;
	    if (binchunk *bp = resumelife<binchunk>(chunkp) ; bp) ylikely {
		if ((rs = bp->len) > 0) ylikely {
		    cint ql = rs ;
		    if (n > 0) {
	                if ((oi + n) < ql) {
	                    al = n ;
	                    oi += al ;
	                } else if (n == (ql - oi)) {
	                    al = (ql - oi) ;
	                    rs = bp->reset ;
	                    oi = 0 ;
			} else {
			    rs = SR_INVALID ;
	                } /* end if */
	            } else if (n < 0) {
	                al = (ql - oi) ;
	                rs = bp->reset ;
	                oi = 0 ;
	            } /* end if (advance-length) */
		} /* end if (binchunk_len) */
	    } /* end if (valid) */
	} /* end if (open) */
	return (al >= 0) ? al : rs ;
} /* end method (bufslide::iadv) */



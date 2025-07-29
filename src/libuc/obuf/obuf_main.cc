/* obuf_main SUPPORT */
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
	obuf

	Description:
	This object facilitates output buffering, but with just a 
	dynamically sized buffer -- no actual output of any sort.

	Names:
	-ctor-		
	operator-int	
	+=

	Synopses:
	obuf(cchar *sbuf = nullptr,int slen = -1) noex
	int add(cchar *sp,int sl = -1) noex
	int adv(int al) noex
	int len() noex
	int operator-int

	Returns:
	>=0		current additional legnth of buffer (beyonf the start)
	<0		error (system-return) 

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

#include	"obuf.hh"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int obuf::istart() noex {
    	fl.open = true ;
    	return SR_OK ;
} /* end method (obuf::istart) */

int obuf::ifinish() noex {
    	cint		rs = ilen() ;
    	fl.open = false ;
    	return rs ;
} /* end method (obuf::ifinish) */

int obuf::push(int ch) noex {
    	int		rs ;
	try {
	    cchar	cha = charconv(ch) ;
	    b.push_back(cha) ;
	    {
		csize	bsize = b.size() ;
	        rs = (intconv(bsize) - oi) ;
	    }
	} catch (...) {
	    rs = SR_NOMEM ;
	}
	return rs ;
} /* end method (obuf::push) */

int obuf::adv(int al) noex {
    	csize		bsize = b.size() ;
	int		rl = 0 ;
	if (al > 0) {
	    cint	sl = intconv(bsize) ;
	    if (sl > (oi + al)) {
	        rl = (sl - oi) ;
	        oi += rl ;
	    } else {
	        rl = (sl - oi) ;
	        oi += rl ;
	        if (rl == 0) {
	            b.clear() ;
	            oi = 0 ;
	        }
	    } /* end if */
	} else if (al < 0) {
	    cint	sl = intconv(bsize) ;
	    if (sl > oi) {
	        rl = (sl - oi) ;
	        oi += rl ;
	    } else {
	        b.clear() ;
	        oi = 0 ;
	    }
	} /* end if */
	return rl ;
} /* end method (obuf::adv) */

void obuf::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("obuf",rs,"fini-finish") ;
	}
} /* end method (obuf::dtor) */

obuf_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case obufmem_start:
		rs = op->istart() ;
	        break ;
	    case obufmem_finish:
	        rs = op->ifinish() ;
	        break ;
	    case obufmem_count:
	        rs = op->ilen() ;
	        break ;
	    case obufmem_len:
	        rs = op->ilen() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (obuf_co::operator) */



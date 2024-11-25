/* obuf SUPPORT */
/* encoding=ISO8859-1 */
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
#include	<new>
#include	<usystem.h>
#include	<localmisc.h>

#include	"obuf.hh"


/* local defines */


/* default name spaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int obuf::push(int ch) noex {
    	int		rs ;
	try {
	    b.push_back(ch) ;
	    rs = (b.size() - oi) ;
	} catch (...) {
	    rs = SR_NOMEM ;
	}
	return rs ;
}

int obuf::adv(int al) noex {
	cint		sl = b.size() ;
	int		rl = 0 ;
	if (al > 0) {
	    if (sl > (oi+al)) {
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
	    if (sl > oi) {
	        rl = (sl - oi) ;
	        oi += rl ;
	    } else {
	        b.clear() ;
	        oi = 0 ;
	    }
	} /* end if */
	return rl ;
}
/* end subroutine (obuf::adv) */

void obuf::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("obuf",rs,"fini-finish") ;
	}
}

obuf_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case obufmem_start:
		rs = SR_OK ;
	        break ;
	    case obufmem_finish:
	        rs = op->ilen() ;
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
}
/* end method (obuf_co::operator) */



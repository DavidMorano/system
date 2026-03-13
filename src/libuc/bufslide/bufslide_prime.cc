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
	bufslide(cchar *sbuf = nullptr,int slen = -1) noex
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
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<localmisc.h>

#include	"bufslide.hh"


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

int bufslide::istart() noex {
    	int		rs = SR_FAULT ;
	if (this) {
	    rs = SR_OK ;
	    oi = 0 ;
    	    fl.open = true ;
	} /* end if (non-null) */
    	return rs ;
} /* end method (bufslide::istart) */

int bufslide::ifinish() noex {
    	int		rs = SR_FAULT ;
	if (this) {
	    rs = SR_NOTOPEN ;
	    if (fl.open) {
    	        rs = ilen() ;
    	        fl.open = false ;
	    }
	} /* end if (non-null) */
    	return rs ;
} /* end method (bufslide::ifinish) */

int bufslide::push(int ch) noex {
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
} /* end method (bufslide::push) */

int bufslide::adv(int al) noex {
    	csize		bsize = b.size() ;
	int		rl = 0 ; /* return-value */
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
} /* end method (bufslide::adv) */

void bufslide::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("bufslide",rs,"fini-finish") ;
	}
} /* end method (bufslide::dtor) */

bufslide_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case bufslidemem_start:
		rs = op->istart() ;
	        break ;
	    case bufslidemem_finish:
	        rs = op->ifinish() ;
	        break ;
	    case bufslidemem_count:
	    case bufslidemem_len:
	        rs = op->ilen() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (bufslide_co::operator) */



/* bufos_prime SUPPORT */
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
	bufos

	Description:
	This object facilitates output buffering, but with just a 
	dynamically sized buffer -- no actual output of any sort.

	Names:
	-ctor-		
	operator-int	
	+=

	Synopses:
	bufos(cchar *sbuf = nullptr,int slen = -1) noex
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
#include	<string_view>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"bufos.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */

using std::string_view ;		/* type */


/* local typedefs */

typedef string_view	strview ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int bufos::istart() noex {
    	int		rs = SR_INVALID ;
	if (! fl.open) ylikely {
	    rs = SR_OK ;
	    oi = 0 ;
    	    fl.open = true ;
	} /* end if (non-null) */
    	return rs ;
} /* end method (bufos::istart) */

int bufos::ifinish() noex {
    	int		rs = SR_NOTOPEN ;
	if (fl.open) ylikely {
    	    rs = ilen() ;
    	    fl.open = false ;
	} /* end if (non-null) */
    	return rs ;
} /* end method (bufos::ifinish) */

int bufos::push(int ch) noex {
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
} /* end method (bufos::push) */

int bufos::add(cchar *sp,int µsl) noex {
	int		rs = SR_FAULT ;
	if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	    rs = SR_OK ;
	    try {
	        strview sv(sp,sl) ;
	        b += sv ;
	        {
	            csize bsize = b.size() ;
	            rs = (intconv(bsize) - oi) ;
	        }
	    } catch (...) {
	        rs = SR_NOMEM ;
	    }
	} /* end if (getlenstr) */
	return rs ;
} /* end method (bufos::add) */

int bufos::at(int ei) const noex {
	csize		bsize = b.size() ;
	int		rs = SR_INVALID ;
	int		rch = 0 ; /* return-value */
	if (ei >= 0) ylikely {
	    cint ql = intconv(bsize) ;
	    rs = SR_NOTFOUND ;
	    if ((oi + ei) < ql) {
		rs = SR_OK ;
		rch = mkchar(b[oi + ei]) ;
	    } /* end if */
	} /* end if (valid) */
	return (rs >= 0) ? rch : rs ;
} /* end method (bufos::at) */

int bufos::readat(int ei,char *rbuf,int rlen) const noex {
    	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (rbuf) ylikely {
	    rs = SR_INVALID ;
	    if (ei >= 0) ylikely {
	        if ((rs = ilen()) > 0) {
		    if (cint cnt = rs ; ei < cnt) {
		        while (rlen-- && (ei < cnt)) {
			    rbuf[rl++] = b[oi + ei++] ;
		        } /* end while */
		    } else {
		        rs = SR_INVALID ;
		    } /* end if */
	        } /* end if (ilen) */
	    } /* end if (valid) */
	    rbuf[rl] = '\0' ;
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
} /* end method (bufos::readat) */

int bufos::read(char *rbuf,int rlen) const noex {
    	return readat(0,rbuf,rlen) ;
} /* end method (bufos::read) */

int bufos::iremove(char *rp) noex {
    	csize		bsize = b.size() ;
    	int		rs = SR_EMPTY ;
	int		rl ;
	if (cint ql = intconv(bsize) ; (rl = (ql - oi)) > 0) {
	    rs = SR_OK ;
	    if (rp) {
		try {
		    cchar c = b.at(oi) ;
		    *rp = c ;
		} catch (...) {
		    rs = SR_BUGCHECK ;
		}
	    } /* end if (caller wants the result) */
	    if (rs >= 0) {
	        oi += 1 ;
	        rl -= 1 ;
	    }
	} /* end if (something to remove) */
	return (rs >= 0) ? rl : rs ;
} /* end method (bufos::iremove) */

int bufos::iadv(int n) noex {
    	csize		bsize = b.size() ;
	int		rs = SR_OK ;
	int		rl = 0 ; /* return-value */
	if (n > 0) {
	    cint ql = intconv(bsize) ;
	    if (n < (ql - oi)) {
		rl = n ;
		oi += rl ;
	    } else if (n == (ql - oi)) {
		rl = (ql - oi) ;
	        b.clear() ;
	        oi = 0 ;
	    } else {
		rs = SR_INVALID ;
	    } /* end if */
	} else if (n < 0) {
	    cint ql = intconv(bsize) ;
	    rl = (ql - oi) ;
	    b.clear() ;
	    oi = 0 ;
	} /* end if */
	return (rs >= 0) ? rl : rs ;
} /* end method (bufos::iadv) */



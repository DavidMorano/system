/* bufslide_aux SUPPORT HEADER */
/* charset=ISO8859-1 */
/* lang=C++98 */

/* Output Buffer */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */
/*******************************************************************************

  	Object:
	bufslide

	Description:
	See the source support file 'README.txt' for a description
	of this object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<string>		/* |string(3c++)| */
#include	<string_view>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"bufslide.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* local namespaces */

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

int bufslide::ilen() const noex {
	csize	bsize = b.size() ;
	return (intconv(bsize) - oi) ;
} /* end method (bufslide::ilen) */

local void bufslide_mems(bufslide *op) noex {
        op->start	(op,bufslidemem_start) ;
        op->finish	(op,bufslidemem_finish) ;
        op->count	(op,bufslidemem_count) ;
        op->len		(op,bufslidemem_len) ;
} /* end subroutine (bufslide_mems) */

bufslide::bufslide(cchar *sp,int sl) noex : oi(0) {
    	bufslide_mems(this) ;
        if (sp) {
            if (sl < 0) {
                sl = lenstr(sp) ;
            }
            try {
                strview sv(sp,sl) ;
                b += sv ;
            } catch (...) {
                (void) sp ;
            }
        } /* end if (non-null) */
} /* end ctor (bufslide) */

int bufslide::operator [] (int i) const noex {
	cint		n = intconv(b.size()) ;
	int		rch = 0 ; /* return-value */
	if ((oi + i) < n) {
	    rch = mkchar(b[oi + i]) ;
	}
	return rch ;
} /* end method (bufslide::operator) */

int bufslide::add(cchar *sp,int µsl) noex {
	int		rs = SR_FAULT ;
	if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	    rs = SR_OK ;
	    try {
	        strview sv(sp,sl) ;
	        b += sv ;
	        {
	            csize	bsize = b.size() ;
	            rs = (intconv(bsize) - oi) ;
	        }
	    } catch (...) {
	        rs = SR_NOMEM ;
	    }
	} /* end if (getlenstr) */
	return rs ;
} /* end method (bufslide::add) */

int bufslide::add(int ch) noex {
    	return push(ch) ;
} /* end method (bufslide::add) */

int bufslide::at(int i) const noex {
	csize	bsize = b.size() ;
	int	rs = SR_INVALID ;
	int	rch = 0 ; /* return-value */
	{
	    cint n = intconv(bsize) ;
	    if ((oi + i) < n) {
		rs = SR_OK ;
		rch = mkchar(b[oi + i]) ;
	    } /* end if */
	} /* end block */
	return (rs >= 0) ? rch : rs ;
} /* end method (bufslide::at) */

int bufslide::readat(int ei,char *rbuf,int rlen) const noex {
    	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (rbuf) {
	    if ((rs = ilen()) > 0) {
		if (cint cnt = rs ; (ei >= 0) && (ei < cnt)) {
		    while (rlen-- && (rl < (cnt - ei))) {
			rbuf[rl++] = b[ei++] ;
		    } /* end while */
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



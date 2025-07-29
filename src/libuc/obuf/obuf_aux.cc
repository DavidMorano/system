/* obuf_aux SUPPORT HEADER */
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
	obuf

	Description:
	See the source support file ('obuf.cc') for a description
	of this object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<string>		/* |string(3c++)| */
#include	<string_view>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"obuf.hh"

import libutil ;

using std::string_view ;

typedef string_view	strview ;


int obuf::ilen() const noex {
	int		rs ;
	{
	    csize	bsize = b.size() ;
	    rs = intconv(bsize) - oi ;
	}
	return rs ;
} /* end method (obuf::ilen) */

obuf::obuf(cchar *sp,int sl) noex : oi(0) {
        start(this,obufmem_start) ;
        finish(this,obufmem_finish) ;
        count(this,obufmem_count) ;
        len(this,obufmem_len) ;
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
} /* end ctor (obuf) */

int obuf::operator [] (int i) const noex {
	cint		n = intconv(b.size()) ;
	int		rch = 0 ; /* return-value */
	if ((oi + i) < n) {
	    rch = mkchar(b[oi + i]) ;
	}
	return rch ;
} /* end method (obuf::operator) */

int obuf::add(cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	if (sl < 0) sl = lenstr(sp) ;
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
	return rs ;
} /* end method (obuf::add) */

int obuf::add(int ch) noex {
    	return push(ch) ;
} /* end method (obuf::add) */

int obuf::at(int i) const noex {
	csize	bsize = b.size() ;
	int	rs = SR_INVALID ;
	int	rch = 0 ; /* return-value */
	{
	    cint n = intconv(bsize) ;
	    if ((oi + i) < n) {
		rs = SR_OK ;
		rch = mkchar(b[oi + i]) ;
	    }
	}
	return (rs >= 0) ? rch : rs ;
} /* end method (obuf::at) */



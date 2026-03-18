/* binchunk SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Binary-Chunk */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This was made from scratch, although it is (very) similar to
	many others that I have made like it.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	binchunk

	Names:
	binchunk_start
	binchunk_load
	binchunk_read
	binchunk_finish

	Description:
	This object facilitates the management of a binary chunk
	of data.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<ulogerror.h>
#include	<umem.hh>
#include	<localmisc.h>

#include	"binchunk.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libu::umem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cint		nents = BINCHUNK_NENTS ;


/* exported variables */


/* exported subroutines */

binchunk::binchunk() noex : ql(0), qe(0) {
	start	(this,binchunkmem_start ) ;
	finish	(this,binchunkmem_finish) ;
	count	(this,binchunkmem_count) ;
	len	(this,binchunkmem_len) ;
	extent	(this,binchunkmem_extent) ;
	reset	(this,binchunkmem_reset) ;
	binbuf = nullptr ;
} /* end ctor (binchunk) */

int binchunk::istart() noex {
    	return SR_OK ;
} /* end method (binchunk:istart) */

int binchunk::ifinish() noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	if (binbuf) {
	    rs1 = umem.free(binbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    binbuf = nullptr ;
	    qe = 0 ;
	    ql = 0 ;
	} /* end if (non-null) */
    	return rs ;
} /* end method (binchunk:ifinish) */

int binchunk::ireset() noex {
	ql = 0 ;
	return SR_OK ;
} /* end method (binchunk::ireset) */

int binchunk::iextend(int amount) noex {
    	int		rs = SR_OK ;
	if ((ql + amount) > qe) {
	    int8_t	*na = nullptr ;
	    if (binbuf) {
	        cint	ne = max((ql + amount),(2 * ql)) ;
		if ((rs = umem.rall(binbuf,(ne + 1),&na)) >= 0) {
		    binbuf = na ;
		    qe = ne ;
		}
	    } else {
	        cint	ne = max((ql + amount),nents) ;
		if ((rs = umem.mall((ne + 1),&na)) >= 0) {
		    binbuf = na ;
		    qe = ne ;
		}
	    } /* end if */
	} /* end if (extension required) */
	return rs ;
} /* end method (binchunk::iextend) */

int8_t binchunk::operator [] (int i) const noex {
    	return at(i) ;
} /* end method (binchunk::operator) */

int8_t binchunk::at(int i) const noex {
	int8_t	rv = 0 ;
	if ((i >= 0) && (i < ql) && binbuf) {
	    rv = binbuf[i] ;
	}
	return rv ;
} /* end method (binchunk::at) */

int binchunk::add(int8_t v) noex {
    	int		rs ;
	cint		vint = int(v) ;
	{
    	    rs = add(vint) ;
	}
    	return rs ;
} /* end method (binchunk::add) */

int binchunk::add(int v) noex {
    	int		rs ;
	if ((rs = iextend(1)) >= 0) {
	    binbuf[ql++] = int8_t(v) ;
	} /* end if (iextend) */
	return (rs >= 0) ? ql : rs ;
} /* end method (binchunk::add) */

int binchunk::add(const int8_t *sp,int µsl) noex {
    	cchar		*cp = ccharp(sp) ;
    	int		rs = SR_FAULT ;
	if (int sl ; (sl = getlenstr(cp,µsl)) >= 0) {
	    rs = SR_OK ;
	    if (sl > 0) {
	        if ((rs = iextend(sl)) >= 0) {
		    memcopy((binbuf + ql),sp,sl) ;
		    ql += sl ;
		    rs = ql ;
	        } /* end if (iextend) */
	    } /* end if (non-zero positive) */
	} /* end if (getlenstr) */
	return (rs >= 0) ? ql : rs ;
} /* end method (binchunk::add) */

int binchunk::get(const int8_t **rpp) noex {
    	int		rs = SR_NOTOPEN ;
	if (binbuf) {
	    rs = SR_OK ;
	    if (rpp) {
		*rpp = binbuf ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? ql : rs ;
} /* end method (binchunk::get) */

int binchunk::readat(int ei,int8_t *rbuf,int rlen) noex {
    	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (rbuf) {
	    rs = SR_INVALID ;
	    if ((rlen >= 0) && (ei >= 0) && (ei < ql)) {
		rs = SR_OK ;
		if ((ql - ei) > 0) {
	            rl = min(rlen,(ql - ei)) ;
		    memcopy(rbuf,(binbuf + ei),rl) ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
} /* end method (binchunk::readat) */

int binchunk::read(int8_t *rbuf,int rlen) noex {
    	return readat(0,rbuf,rlen) ;
} /* end method (binchunk::read) */

void binchunk::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("binchunk",rs,"fini-finish") ;
	}
} /* end method (binchunk::dtor) */

binchunk::operator int () noex {
	return ql ;
} /* end method (binchunk::operator) */

binchunk_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case binchunkmem_start:
	        rs = op->istart() ;
	        break ;
	    case binchunkmem_finish:
	        rs = op->ifinish() ;
	        break ;
	    case binchunkmem_count:
	    case binchunkmem_len:
	        rs = op->ql ;
	        break ;
	    case binchunkmem_extent:
	        rs = op->qe ;
	        break ;
	    case binchunkmem_reset:
	        rs = op->ireset() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (binchunk_co::operator) */



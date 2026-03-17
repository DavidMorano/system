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
#include	<ulogerror.h>
#include	<binchunk.hh>
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

local void bufslide_mems(bufslide *op) noex {
        op->start	(op,bufslidemem_start) ;
        op->finish	(op,bufslidemem_finish) ;
        op->adv		(op,bufslidemem_adv) ;
        op->count	(op,bufslidemem_count) ;
        op->len		(op,bufslidemem_len) ;
	op->extent	(op,bufslidemem_extent) ;
} /* end subroutine (bufslide_mems) */

bufslide::bufslide() noex : oi(0) {
    	bufslide_mems(this) ;
	chunkp = nullptr ;
} /* end ctor (bufslide) */

int bufslide::ilen() const noex {
    	int		rs = SR_NOTOPEN ;
	if (fl.open) ylikely {
    	    rs = SR_BUGCHECK ;
	    if (binchunk *bp = resumelife<binchunk>(chunkp) ; bp) ylikely {
	        if ((rs = bp->len) >= 0) {
		    if (cint bsz = rs ; bsz >= oi) ylikely {
	                rs = (bsz - oi) ;
		    } else {
		        rs = SR_BUGCHECK ;
		    }
	        } /* end if (chunk_len) */
	    } /* end if (non-null) */
	} /* end if (open) */
	return rs ;
} /* end method (bufslide::ilen) */

int bufslide::iextent() const noex {
    	int		rs = SR_NOTOPEN ;
	if (fl.open) {
    	    rs = SR_BUGCHECK ;
	    if (binchunk *bp = resumelife<binchunk>(chunkp) ; bp) ylikely {
	        rs = bp->len ;
	    } /* end if (non-null) */
	} /* end if (open) */
	return rs ;
} /* end method (bufslide::iextent) */

int bufslide::operator [] (int i) const noex {
    	return at(i) ;
} /* end method (bufslide::operator) */

void bufslide::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("bufslide",rs,"fini-finish") ;
	}
} /* end method (bufslide::dtor) */

int bufslide_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case bufslidemem_start:
		rs = op->istart() ;
	        break ;
	    case bufslidemem_finish:
	        rs = op->ifinish() ;
	        break ;
	    case bufslidemem_adv:
	        rs = op->iadv(a) ;
	        break ;
	    case bufslidemem_count:
	    case bufslidemem_len:
	        rs = op->ilen() ;
	        break ;
	    case bufslidemem_extent:
	        rs = op->iextent() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (bufslide_co::operator) */



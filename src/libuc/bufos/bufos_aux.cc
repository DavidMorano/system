/* bufos_aux SUPPORT HEADER */
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
	bufos

	Description:
	See the source support file ('bufos.cc') for a description
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
#include	<mkchar.h>
#include	<localmisc.h>

#include	"bufos.hh"

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

local void bufos_mems(bufos *op) noex {
        op->start	(op,bufosmem_start) ;
        op->finish	(op,bufosmem_finish) ;
        op->adv		(op,bufosmem_adv) ;
        op->count	(op,bufosmem_count) ;
        op->len		(op,bufosmem_len) ;
        op->extent	(op,bufosmem_extent) ;
	op->rem		(op,0) ;
} /* end subroutine (bufos_mems) */

bufos::bufos(cchar *sp,int sl) noex : oi(0) {
    	bufos_mems(this) ;
        if (sp) ylikely {
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
} /* end ctor (bufos) */

int bufos::ilen() const noex {
	csize	bsize = b.size() ;
	return (intconv(bsize) - oi) ;
} /* end method (bufos::ilen) */

int bufos::iextent() const noex {
	csize	bsize = b.size() ;
	return intconv(bsize) ;
} /* end method (bufos::iextent) */

int bufos::operator [] (int ei) const noex {
	cint		n = intconv(b.size()) ;
	int		rch = 0 ; /* return-value */
	if ((oi + ei) < n) {
	    rch = mkchar(b[oi + ei]) ;
	}
	return rch ;
} /* end method (bufos::operator) */

void bufos::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("bufos",rs,"fini-finish") ;
	}
} /* end method (bufos::dtor) */

int bufos_rem::operator () (char *rp) noex {
    	return op->iremove(rp) ;
} /* end method (bufos_rem::operator) */

int bufos_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case bufosmem_start:
		rs = op->istart() ;
	        break ;
	    case bufosmem_finish:
	        rs = op->ifinish() ;
	        break ;
	    case bufosmem_adv:
	        rs = op->iadv(a) ;
	        break ;
	    case bufosmem_count:
	    case bufosmem_len:
	        rs = op->ilen() ;
	        break ;
	    case bufosmem_extent:
	        rs = op->iextent() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (bufos_co::operator) */



/* outbuf SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* output buffer management (really?) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-02, David A­D­ Morano
	This module was originally written to replace the old (yack)
	'outbuf' mechanism.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	outbuf

	Description:
	This little object lets a caller start out thinking that
	his caller-supplied buffer will be returned by the |_get()|
	method.  However, if the original call to the |_start()|
	method specified a |nullptr| buffer, then one is allocated and
	returned.  In either case, a call to the |_finish()| method
	will deallocate any allocated buffer.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<localmisc.h>

#include	"outbuf.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int outbuf_start(outbuf *op,char *obuf,int olen) noex {
	int		rs = SR_FAULT ;
	if (op && obuf) ylikely {
	    op->obuf = nullptr ;
	    op->olen = 0 ;
	    op->f_alloc = false ;
	    rs = SR_INVALID ;
	    if (olen != 0) {
	        rs = SR_OK ;
	        op->obuf = obuf ;
	        if (olen >= 0) {
	            op->olen = olen ;
	        } else {
		    if ((rs = maxpathlen) >= 0) {
	               op->olen = rs ;
		    }
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (outbuf_start) */

int outbuf_finish(outbuf *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->f_alloc && op->obuf) {
	        rs1 = uc_free(op->obuf) ;
	        if (rs >= 0) rs = rs1 ;
	        op->obuf = nullptr ;
	    }
	    op->f_alloc = false ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (outbuf_finish) */

int outbuf_get(outbuf *op,cchar **onpp) noex {
	int		rs = SR_FAULT ;
	if (op && onpp) ylikely {
	    rs = SR_OK ;
	    if (op->f_alloc) {
	        op->obuf[0] = '\0' ;
	        *onpp = op->obuf ;
	    } else {
	        if (op->obuf == nullptr) {
		    cint	sz = (op->olen + 1) ;
	            if (char *vp{} ; (rs = uc_valloc(sz,&vp)) >= 0) {
	                op->obuf = vp ;
	                op->f_alloc = true ;
	                op->obuf[0] = '\0' ;
	                *onpp = op->obuf ;
	            }
	        } else {
	            op->obuf[0] = '\0' ;
	            *onpp = op->obuf ;
	            rs = SR_OK ;
	        } /* end if */
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (outbuf_get) */

int outbuf::get(cchar **rpp) noex {
	return outbuf_get(this,rpp) ;
}

void outbuf::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("outbuf",rs,"dtor-finish") ;
	}
}

outbuf::operator int () noex {
    	return olen ;
}

outbuf_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case outbufmem_finish:
	        rs = outbuf_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (outbuf_co::operator) */




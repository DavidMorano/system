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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<bufsizevar.hh>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"outbuf.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local variables */

static bufsizevar	maxpathlen(bufsize_mp) ;


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int outbuf_start(outbuf *op,char *obuf,int olen) noex {
	int		rs = SR_FAULT ;
	if (op && obuf) ylikely {
	    op->obuf = nullptr ;
	    op->olen = 0 ;
	    op->falloc = false ;
	    rs = SR_INVALID ;
	    if (olen != 0) ylikely {
	        rs = SR_OK ;
	        op->obuf = obuf ;
	        if (olen >= 0) {
	            op->olen = olen ;
	        } else {
		    if ((rs = maxpathlen) >= 0) {
	               op->olen = rs ;
		    }
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (outbuf_start) */

int outbuf_finish(outbuf *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->falloc && op->obuf) ylikely {
	        rs1 = lm_free(op->obuf) ;
	        if (rs >= 0) rs = rs1 ;
	        op->obuf = nullptr ;
	    } /* end if (memory-release) */
	    op->falloc = false ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (outbuf_finish) */

int outbuf_get(outbuf *op,cchar **onpp) noex {
	int		rs = SR_FAULT ;
	if (op && onpp) ylikely {
	    rs = SR_OK ;
	    if (op->falloc) {
	        op->obuf[0] = '\0' ;
	        *onpp = op->obuf ;
	    } else {
	        if (op->obuf == nullptr) {
		    cint	sz = (op->olen + 1) ;
	            if (char *vp ; (rs = lm_vall(sz,&vp)) >= 0) {
	                op->obuf = vp ;
	                op->falloc = true ;
	                op->obuf[0] = '\0' ;
	                *onpp = op->obuf ;
	            } /* end if (memory-acquire) */
	        } else {
	            op->obuf[0] = '\0' ;
	            *onpp = op->obuf ;
	            rs = SR_OK ;
	        } /* end if */
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (outbuf_get) */

int outbuf::start(char *ob,int ol) noex {
	return outbuf_start(this,ob,ol) ;
} /* end method */

int outbuf::get(cchar **rpp) noex {
	return outbuf_get(this,rpp) ;
} /* end method */

void outbuf::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("outbuf",rs,"dtor-finish") ;
	}
} /* end method (outbuf::dtor) */

outbuf::operator int () noex {
    	return olen ;
} /* end method (outbuf::operator) */

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
} /* end method (outbuf_co::operator) */



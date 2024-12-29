/* outbuf SUPPORT */
/* encoding=ISO8859-1 */
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
#include	<unistd.h>
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

int outbuf_start(outbuf *oop,char *obuf,int olen) noex {
	int		rs = SR_FAULT ;
	if (oop && obuf) {
	    rs = SR_INVALID ;
	    if (olen != 0) {
	        rs = SR_OK ;
	        oop->f_alloc = false ;
	        oop->obuf = obuf ;
	        if (olen >= 0) {
	            oop->olen = olen ;
	        } else {
		    if ((rs = maxpathlen) >= 0) {
	               oop->olen = rs ;
		    }
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (outbuf_start) */

int outbuf_finish(outbuf *oop) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (oop) {
	    rs = SR_OK ;
	    if (oop->f_alloc && oop->obuf) {
	        rs1 = uc_free(oop->obuf) ;
	        if (rs >= 0) rs = rs1 ;
	        oop->obuf = nullptr ;
	    }
	    oop->f_alloc = false ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (outbuf_finish) */

int outbuf_get(outbuf *oop,char **onpp) noex {
	int		rs = SR_FAULT ;
	if (oop && onpp) {
	    rs = SR_OK ;
	    if (oop->f_alloc) {
	        oop->obuf[0] = '\0' ;
	        *onpp = oop->obuf ;
	    } else {
	        if (oop->obuf == nullptr) {
		    cint	sz = (oop->olen + 1) ;
	            if (char *vp{} ; (rs = uc_valloc(sz,&vp)) >= 0) {
	                oop->obuf = vp ;
	                oop->f_alloc = true ;
	                oop->obuf[0] = '\0' ;
	                *onpp = oop->obuf ;
	            }
	        } else {
	            oop->obuf[0] = '\0' ;
	            *onpp = oop->obuf ;
	            rs = SR_OK ;
	        } /* end if */
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (outbuf_get) */



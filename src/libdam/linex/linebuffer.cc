/* linebuffer SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* provide a line-buffer of the system-defined line-length */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	linebuffer

	Description:
	This object module (linebuffer) provides a dynamically sized
	line-bufefer of the system-defined line-length.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<localmisc.h>

#include	"linebuffer.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int linebuffer_start(linebuffer *op) noex {
	int		rs = SR_FAULT ;
	int		cl = 0 ;
	if (op) ylikely {
	    if (char *cp ; (rs = malloc_ml(&cp)) >= 0) ylikely {
		cl = rs ;
		op->lbuf = cp ;
		op->llen = rs ;
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return (rs >= 0) ? cl : rs ;
}
/* end subroutine (linebuffer_start) */

int linebuffer_finish(linebuffer *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->lbuf) {
		op->lbuf[0] = '\0' ;
	        rs1 = uc_free(op->lbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        op->lbuf = nullptr ;
		op->llen = 0 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (linebuffer_finish) */


/* local subroutines */

linebuffer_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case linebuffermem_start:
	        rs = linebuffer_start(op) ;
	        break ;
	    case linebuffermem_finish:
	        rs = linebuffer_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (linebuffer_co::operator) */

void linebuffer::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("linebuffer",rs,"fini-finish") ;
	}
} /* end method (linebuffer::dtor) */



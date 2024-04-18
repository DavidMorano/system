/* linebuffer SUPPORT */
/* lang=C++20 */

/* provide a line-buffer of the system-defined line-length */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object module (linebuffer) provides a dynamically sized
	line-bufefer of the system-defined line-length.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<localmisc.h>

#include	"linebuffer.h"


/* local defines */


/* external subroutines */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int linebuffer_start(linebuffer *op) noex {
	int		rs = SR_FAULT ;
	int		cl = 0 ;
	if (op) {
	    char	*cp ;
	    if ((rs = malloc_ml(&cp)) >= 0) {
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
	if (op) {
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
	switch (w) {
	case linebuffermem_start:
	    rs = linebuffer_start(op) ;
	    break ;
	case linebuffermem_finish:
	    rs = linebuffer_finish(op) ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end method (linebuffer_co::operator) */



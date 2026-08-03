/* rebuf SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* this is a resuable (fixed-sized) buffer */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debug print-outs */

/* revision history:

	= 1998-09-10, David A­D­ Morano
	This module was changed to serve in the REFERM program.

*/

/* Copyright © 1998,2005 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	rebuf

	Description:
	This object implements a reusuable fixed-sized buffer.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ulogerror.h>		/* LIBU */
#include	<sbuf.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG */

#include	"rebuf.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */


/* local subroutines */

int rebuf::start(char *rb,int rl) noex {
	int		rs = SR_FAULT ;
	if (rb) ylikely {
	    if ((rs = sbuf::start(rb,rl)) >= 0) ylikely {
		magval = REBUF_MAGIC ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end method (rebuf::start) */

void rebuf::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("rebuf",rs,"fini-finish") ;
	}
} /* end method (rebuf::dtor) */

rebuf_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case rebufmem_bufbeg:
    		rs = op->reset ;
	        break ;
	    case rebufmem_bufend:
    		rs = op->reset ;
	        break ;
	    case rebufmem_finish:
	        rs = op->sbuf::finish ;
		op->magval = 0 ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (rebuf_co::operator) */



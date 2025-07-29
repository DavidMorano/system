/* lineclean SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* clean up a line of text w/ options */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	lineclean

  	Description:
	This small object cleans up a line of text (w/ options).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* placement-new + |nothrow(3c++)| */
#include	<usystem.h>
#include	<mkx.h>			/* |mklineclean(3uc)| */
#include	<localmisc.h>

#include	"lineclean.h"

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;

/* local defines */

#define	LC		lineclean
#define	LC_ARGS		lineclean_args


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int lineclean_ctor(lineclean *op,Args ... args) noex {
    	LINECLEAN	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (lineclean_ctor) */

static int lineclean_dtor(lineclean *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (lineclean_dtor) */

template<typename ... Args>
static inline int lineclean_magic(lineclean *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == LINECLEAN_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (lineclean_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int lineclean_start(LC *op,cchar *sp,int sl,int m,cchar **rpp) noex {
	int		rs ;
	if ((rs = lineclean_ctor(op,sp,rpp)) >= 0) {
	    if (sl < 0) sl = lenstr(sp) ;
	    if (sl > 0) {
		cint	sz = (sl + 1) ;
		if (void *vp ; (rs = uc_malloc(sz,&vp)) >= 0) {
		    op->cbuf = charp(vp) ;
		    op->clen = sl ;
		    if ((rs = mklineclean(op->cbuf,op->clen,m,sp,sl)) >= 0) {
			*rpp = op->cbuf ;
		        op->magic = LINECLEAN_MAGIC ;
		    } /* end if (mklineclean) */
		    if (rs < 0) {
			uc_free(op->cbuf) ;
			op->cbuf = nullptr ;
			op->clen = 0 ;
		    } /* end if (error) */
		} /* end if (memory-allocation) */
	    } else {
		*rpp = op->cbuf ;
		op->magic = LINECLEAN_MAGIC ;
	    } /* end if (zero-length) */
	    if (rs < 0) {
		lineclean_dtor(op) ;
	    }
	} /* end if (lineclean_ctor) */
	return rs ;
} /* end subroutine (lineclean_start) */

int lineclean_finish(LC *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = lineclean_magic(op)) >= 0) {
	    if (op->cbuf) {
	        rs1 = uc_free(op->cbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        op->cbuf = nullptr ;
		op->clen = 0 ;
	    }
	    {
	        rs1 = lineclean_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (lineclean_finish) */


/* private subroutines */

int lineclean::start(cchar *sp,int sl,int m,cchar **rpp) noex {
	return lineclean_start(this,sp,sl,m,rpp) ;
} /* end method (lineclean::start) */

void lineclean::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("lineclean",rs,"fini-finish") ;
	}
} /* end method (lineclean::dtor) */

lineclean_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case linecleanmem_finish:
	        rs = lineclean_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (lineclean_co::operator) */



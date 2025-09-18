/* prefixfn SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* create an absolute file-name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	prefixfn
	
	Description:
	This object module (prefixfn) creates an absolute file-name
	when needed.  It does this by prepending the present 
	working directory (PWD) to those file names that a relative.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<libmallocxx.h>
#include	<mkx.h>			/* |mkpathexp(3uc)| */
#include	<mkchar.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"prefixfn.h"

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	prefixfn_loadexp(prefixfn *,cchar *,int,cchar **) noex ;
local int	prefixfn_loadnul(prefixfn *,cchar *,int,cchar **) noex ;

local bool isprefix(int ch) noex {
    	cint	ch_tilde	= CH_TILDE ;
    	cint	ch_var		= mkchar('¬') ;
    	return ((ch == ch_tilde) || (ch == ch_var)) ;
} /* end subroutine (isprefix) */


/* local variables */

constexpr cauto		mem_almp = libmalloc_mp ;
constexpr cauto		mem_alst = lm_strw ;
constexpr cauto		mem_free = lm_free ;


/* exported variables */


/* exported subroutines */

int prefixfn_start(prefixfn *op,cchar *sp,int µsl,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		sl = 0 ; /* return-value */
	if (op && sp && rpp) ylikely {
	    rs = SR_INVALID ;
	    op->as = nullptr ;
	    if ((sl = getlenstr(sp,µsl)) > 0) {
		cint	chx_prefix = mkchar(sp[0]) ;
	        rs = SR_OK ;
	        *rpp = sp ;
	        if (isprefix(chx_prefix)) {
		    rs = prefixfn_loadexp(op,sp,sl,rpp) ;
		    sl = rs ;
		} else if (sp[sl] != '\0') {
		    rs = prefixfn_loadnul(op,sp,sl,rpp) ;
		    sl = rs ;
		}
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (prefixfn_start) */

int prefixfn_finish(prefixfn *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->as) {
		char *bp = cast_const<charp>(op->as) ;
	        rs1 = mem_free(bp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->as = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (prefixfn_finish) */


/* local subroutines */

local int prefixfn_loadexp(prefixfn *op,cchar *sp,int sl,cchar **rpp) noex {
	int		rs ;
	int		rs1 ;
	if (char *pbuf ; (rs = mem_almp(&pbuf)) >= 0) ylikely {
	    if ((rs = mkpathexp(pbuf,sp,sl)) > 0) {
		if (cchar *cp ; (rs = mem_alst(pbuf,rs,&cp)) >= 0) {
		    sl = rs ;
		    op->as = cp ;
		    *rpp = cp ;
		}
	    } else if (rs == 0) {
		if (sp[sl] != '\0') {
		    rs = prefixfn_loadnul(op,sp,sl,rpp) ;
		    sl = rs ;
		} else {
		    *rpp = sp ;
		}
	    } /* end if (mkpathexp) */
	    rs1 = mem_free(pbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (prefixfn_loadexp) */

local int prefixfn_loadnul(prefixfn *op,cchar *sp,int sl,cchar **rpp) noex {
	int		rs ;
	if (cchar *cp ; (rs = mem_alst(sp,sl,&cp)) >= 0) {
	    sl = rs ;
	    op->as = cp ;
	    *rpp = cp ;
	} /* end if (memory-allocation) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (prefixfn_loadnul) */

int prefixfn::start(cchar *sp,int sl,cchar **rpp) noex {
	return prefixfn_start(this,sp,sl,rpp) ;
}

void prefixfn::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("prefixfn",rs,"fini-finish") ;
	}
} /* end method (prefixfn::dtor) */

prefixfn_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case prefixfnmem_finish:
	        rs = prefixfn_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (prefixfn_co::operator) */



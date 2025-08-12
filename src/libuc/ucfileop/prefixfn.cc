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
#include	<mallocxx.h>
#include	<getpwd.h>
#include	<strwcpy.h>
#include	<pathadd.h>
#include	<localmisc.h>

#include	"prefixfn.h"

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */

static int	prefixfn_loadpwd(prefixfn *,cchar *,int,cchar **) noex ;
static int	prefixfn_loadnul(prefixfn *,cchar *,int,cchar **) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int prefixfn_start(prefixfn *op,cchar *sp,int µsl,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		sl = 0 ; /* return-value */
	if (op && sp && rpp) ylikely {
	    rs = SR_INVALID ;
	    op->as = nullptr ;
	    if ((sl = getlenstr(sp,µsl)) > 0) {
	        rs = SR_OK ;
	        *rpp = sp ;
	        if (sp[0] != '/') {
		    rs = prefixfn_loadpwd(op,sp,sl,rpp) ;
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
	        rs1 = uc_libfree(bp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->as = nullptr ;
	    }
	    op->buf[0] = '\0' ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (prefixfn_finish) */


/* local subroutines */

static int prefixfn_loadpwd(prefixfn *op,cchar *sp,int sl,cchar **rpp) noex {
	int		rs ;
	int		rs1 ;
	if (char *pbuf ; (rs = malloc_mp(&pbuf)) >= 0) ylikely {
	    cint	plen = rs ;
	    if ((rs = getpwd(pbuf,plen)) >= 0) ylikely {
	 	if ((rs = pathaddw(pbuf,rs,sp,sl)) >= 0) ylikely {
		    sl = rs ;
	            if (sl > PREFIXFN_SHORTLEN) {
	                if (cc *cp ; (rs = uc_libmallocstrw(sp,sl,&cp)) >= 0) {
	                    *rpp = cp ;
	                    op->as = cp ;
	                } /* end if (memory-allocation) */
	            } else {
	                *rpp = op->buf ;
	                strwcpy(op->buf,sp,sl) ;
	            } /* end if */
	        } /* end if (pathadd) */
	    } /* end if (getpwd) */
	    rs1 = uc_free(pbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (prefixfn_loadpwd) */

static int prefixfn_loadnul(prefixfn *op,cchar *sp,int sl,cchar **rpp) noex {
	int		rs = SR_OK ;
	if (sl > PREFIXFN_SHORTLEN) {
	    if (cchar *cp ; (rs = uc_libmallocstrw(sp,sl,&cp)) >= 0) {
		sl = rs ;
		*rpp = cp ;
		op->as = cp ;
	    } /* end if (memory-allocation) */
	} else {
	    *rpp = op->buf ;
	    strwcpy(op->buf,sp,sl) ;
	} /* end if */
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



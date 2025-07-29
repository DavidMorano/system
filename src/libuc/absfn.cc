/* absfn SUPPORT */
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
	absfn
	
	Description:
	This object module (absfn) creates an absolute file-name
	when needed.  It does this by prepending the present 
	working directory (PWD) to those file names that a relative.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- for |lenstr(3c)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<getpwd.h>
#include	<strwcpy.h>
#include	<pathadd.h>
#include	<localmisc.h>

#include	"absfn.h"

import libutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */

static int	absfn_loadpwd(absfn *,cchar *,int,cchar **) noex ;
static int	absfn_loadnul(absfn *,cchar *,int,cchar **) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int absfn_start(absfn *op,cchar *sp,int sl,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (op && sp && rpp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    rs = SR_OK ;
	    op->as = nullptr ;
	    *rpp = sp ;
	    if (sl > 0) {
	        if (sp[0] != '/') {
		    rs = absfn_loadpwd(op,sp,sl,rpp) ;
		    sl = rs ;
		} else if (sp[sl] != '\0') {
		    rs = absfn_loadnul(op,sp,sl,rpp) ;
		    sl = rs ;
		}
	    } /* end if (PWD needed) */
	} /* end if (non-null) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (absfn_start) */

int absfn_finish(absfn *op) noex {
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
/* end subroutine (absfn_finish) */


/* local subroutines */

static int absfn_loadpwd(absfn *op,cchar *sp,int sl,cchar **rpp) noex {
	int		rs ;
	int		rs1 ;
	if (char *pbuf ; (rs = malloc_mp(&pbuf)) >= 0) ylikely {
	    cint	plen = rs ;
	    if ((rs = getpwd(pbuf,plen)) >= 0) ylikely {
	 	if ((rs = pathaddw(pbuf,rs,sp,sl)) >= 0) ylikely {
		    sl = rs ;
	            if (sl > ABSFN_SHORTLEN) {
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
/* end subroutine (absfn_loadpwd) */

static int absfn_loadnul(absfn *op,cchar *sp,int sl,cchar **rpp) noex {
	int		rs = SR_OK ;
	if (sl > ABSFN_SHORTLEN) {
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
/* end subroutine (absfn_loadnul) */

int absfn::start(cchar *sp,int sl,cchar **rpp) noex {
	return absfn_start(this,sp,sl,rpp) ;
}

void absfn::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("absfn",rs,"fini-finish") ;
	}
} /* end method (absfn::dtor) */

absfn_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case absfnmem_finish:
	        rs = absfn_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (absfn_co::operator) */



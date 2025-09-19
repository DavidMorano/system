/* posname SUPPORT (POSIX® Entity name) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* create a POSIX® entify name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	posname
	
	Description:
	This object module (POSNAME) creates a POSIX® entity name
	when needed.  It does this by possibly prepending a slash
	('/') character before the given name.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<sncpyxw.h>		/* |sncpy2w(3uc)| */
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"posname.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ulibvals.ccm"

import libutil ;			/* |getlenstr(3u)| */
import ulibvals ;			/* |maxpathlen(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */

static int	posname_loadslash(posname *,cchar *,int,cchar **) noex ;
static int	posname_loadnul(posname *,cchar *,int,cchar **) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int posname_start(posname *op,cchar *sp,int µsl,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (op && sp && rpp) ylikely {
	    rs = SR_INVALID ;
	    op->as = nullptr ;
	    if (int sl ; (sl = getlenstr(sp,µsl)) > 0) {
	        *rpp = sp ;
		rs = SR_OK ;
	        if (sp[0] != '/') {
		    rs = posname_loadslash(op,sp,sl,rpp) ;
		    rl = rs ;
		} else if (sp[sl] != '\0') {
		    rs = posname_loadnul(op,sp,sl,rpp) ;
		    rl = rs ;
		} else {
		    rl = sl ;
		}
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (posname_start) */

int posname_finish(posname *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->as) ylikely {
		char *bp = cast_const<charp>(op->as) ;
		bp[0] = '\0' ;
	        rs1 = lm_free(bp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->as = nullptr ;
	    }
	    op->buf[0] = '\0' ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (posname_finish) */


/* local subroutines */

static int posname_loadslash(posname *op,cchar *sp,int sl,cchar **rpp) noex {
	int		rs ;
	int		rs1 ;
	int		nl = 0 ; /* return-value */
	if ((rs = ulibval.maxpathlen) >= 0) ylikely {
	    cint	nlen = rs ;
	    if (char *nbuf ; (rs = lm_mall((nlen + 1),&nbuf)) >= 0) ylikely {
	        if ((rs = sncpy2w(nbuf,nlen,"/",sp,sl)) >= 0) ylikely {
		    if ((nl = rs) > POSNAME_SHORTLEN) {
		        if (cc *cp ; (rs = lm_strw(nbuf,nl,&cp)) >= 0) ylikely {
	                    op->as = cp ;
	                    *rpp = cp ;
	                } /* end if (memory-allocation) */
	            } else {
	                *rpp = op->buf ;
	                strwcpy(op->buf,nbuf,nl) ;
	            } /* end if */
	        } /* end if (pathadd) */
	        rs1 = lm_free(nbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (ulibval.maxpathlen) */
	return (rs >= 0) ? nl : rs ;
}
/* end subroutine (posname_loadslash) */

static int posname_loadnul(posname *op,cchar *sp,int sl,cchar **rpp) noex {
	int		rs = SR_OK ;
	int		nl = 0 ; /* return-value */
	if (sl > POSNAME_SHORTLEN) {
	    if (cchar *cp ; (rs = lm_strw(sp,sl,&cp)) >= 0) ylikely {
		op->as = cp ;
		nl = rs ;
		*rpp = cp ;
	    } /* end if (memory-allocation) */
	} else {
	    *rpp = op->buf ;
	    strwcpy(op->buf,sp,sl) ;
	    nl = rs ;
	} /* end if */
	return (rs >= 0) ? nl : rs ;
}
/* end subroutine (posname_loadnul) */

int posname::start(cchar *sp,int sl,cchar **rpp) noex {
	return posname_start(this,sp,sl,rpp) ;
}

void posname::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("posname",rs,"fini-finish") ;
	}
} /* end method (posname::dtor) */

posname_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case posnamemem_finish:
	        rs = posname_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (posname_co::operator) */



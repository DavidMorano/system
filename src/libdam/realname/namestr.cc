/* namestr SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* manipulate real names */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-13, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Object:
	namestr

	Description:
	This object manages name-strings.

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<new>
#include	<usystem.h>
#include	<six.h>
#include	<snwcpyx.h>
#include	<char.h>		/* |CHAR_ISWHITE(3uc)| */
#include	<ischarx.h>
#include	<localmisc.h>		/* |REALNAME| */

#include	"namestr.h"


/* local defines */


/* imported nameopaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int namestr_start(namestr *op,cchar *sbuf,int slen) noex {
    	int		rs = SR_FAULT ;
	if (op && sbuf) {
	    rs = SR_OK ;
	    op->strp = sbuf ;
	    op->strl = (slen >= 0) ? slen : xstrlen(sbuf) ;
	}
	return rs ;
}
/* end subroutine (namestr_start) */

int namestr_finish(namestr *op) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->strp = nullptr ;
	    op->strl = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (namestr_finish) */

int namestr_skipwhite(namestr *op) noex {
	while ((op->strl > 0) && op->strp[0] && CHAR_ISWHITE(op->strp[0])) {
	    op->strp += 1 ;
	    op->strl -= 1 ;
	}
	return op->strl ;
}
/* end subroutine (namestr_skipwhite) */

int namestr_brk(namestr *op,cchar *bs,cchar **rpp) noex {
	cint		si = sibreak(op->strp,op->strl,bs) ;
	*rpp = (si >= 0) ? (op->strp + si) : nullptr ;
	return si ;
}
/* end subroutine (namestr_brk) */

int namestr_next(namestr *op,cchar **npp,int *fap,int *flp) noex {
	int		rs = SR_EOF ;
	int		nlen = 0 ;
	*fap = false ;
	*flp = false ;
	namestr_skipwhite(op) ;
	if (op->strl > 0) {
	    cchar	*sxp ;
	    cchar	*cp ;
	    rs = SR_OK ;
	    if (int i ; (i = namestr_brk(op," \t.,­",&cp)) >= 0) {
	        sxp = op->strp ;
	        nlen = i ;
	        op->strp += nlen ;
	        op->strl -= nlen ;
	        namestr_skipwhite(op) ;
	        bool	f = true ;
	        f = f && (op->strl > 0) ;
	        f = f && ((op->strp[0] == ',') || isAbbr(op->strp[0])) ;
	        if (f) {
	            if (isAbbr(op->strp[0])) {
	                *fap = true ;
	            } else {
	                *flp = true ;
		    }
	            op->strp += 1 ;
	            op->strl -= 1 ;
	        } /* end if */
	        namestr_skipwhite(op) ;
	        if ((! *flp) && (op->strl > 0) && (op->strp[0] == ',')) {
	            *flp = true ;
	            op->strp += 1 ;
	            op->strl -= 1 ;
	        } /* end if */
	        namestr_skipwhite(op) ;
    		/* eat any weirdo characters that are here */
	        while ((op->strl > 0) && 
	            ((op->strp[0] == ',') || (op->strp[0] == '.'))) {
	            op->strp += 1 ;
	            op->strl -= 1 ;
	        } /* end while */
	    } else {
	        sxp = op->strp ;
	        nlen = (op->strl > 0) ? op->strl : SR_EOF ;
	        op->strp += nlen ;
	        op->strl = 0 ;
	    } /* end if */
	    if (npp) {
	        *npp = (nlen >= 0) ? sxp : nullptr ;
	    }
	} /* end if (valid) */
	return (rs >= 0) ? nlen : rs ;
}
/* end subroutine (namestr_next) */

int namestr::start(cchar *sp,int sl) noex {
    	return namestr_start(this,sp,sl) ;
}

int namestr::next(cchar **npp,int *fap,int *flp) noex {
	return namestr_next(this,npp,fap,flp) ;
}

int namestr::brk(cchar *bs,cchar **rpp) noex {
	return namestr_brk(this,bs,rpp) ;
}

void namestr::dtor() noex {
	if (cint rs = int(finish) ; rs < 0) {
	    ulogerror("namestr",rs,"fini-finish") ;
	}
}

namestr_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case namestrmem_skipwhite:
	        rs = namestr_skipwhite(op) ;
	        break ;
	    case namestrmem_finish:
	        rs = namestr_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (namestr_co::operator) */



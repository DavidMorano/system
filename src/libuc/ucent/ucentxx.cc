/* ucentxx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UCENTXX object management support routines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	si_{x}

	Description:
	These are utility subroutines for the UCENT group within
	the UCSYS subsystem (itself in the UC library).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<storeitem.h>
#include	<sbuf.h>
#include	<vechand.h>
#include	<strn.h>
#include	<sfx.h>
#include	<six.h>
#include	<localmisc.h>

#include	"ucentxx.hh"


/* local defines */

#define	SI	storeitem


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* local variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

#ifdef	COMMENT /* currently unused */
namespace ucent {
    int sbuf_fmtstr(sbuf *sbp,cchar *sp) noex {
	cint		clen = 16 ;
	int		rs ;
	if ((rs = sbp->getlen) >= 0) {
	    cint	idx = rs ;
	    cint	nb = (idx >= clen) ? 1 : (clen-idx) ;
	    if ((rs = sbp->blanks(nb)) >= 0) {
		rs = sbp->str(sp) ;
	    }
	}
	return rs ;
    } /* end subroutine (sbuf_fmtstr) */
}
#endif /* COMMENT (currently unused) */

namespace ucent {
    int si_loadnames(SI *sip,vechand *ulp,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar		*tp ;
	while ((tp = strnbrk(sp,sl," ,")) != nullptr) {
	    if (cint tl = intconv(tp - sp) ; tl > 0) {
		c += 1 ;
		rs = si_loadname(sip,ulp,sp,tl) ;
	    } /* end if (non-zero) */
	    sl -= intconv((tp + 1) - sp) ;
	    sp = (tp + 1) ;
	    if (rs < 0) break ;
	} /* end while */
	if ((rs >= 0) && sl && sp[0]) {
	    c += 1 ;
	    rs = si_loadname(sip,ulp,sp,sl) ;
	}
	return (rs >= 0) ? c : rs ;
    } /* end subroutine (si_loadnames) */
}

namespace ucent {
    int si_loadname(SI *sip,vechand *ulp,cchar *sp,int sl) noex {
	int		rs ;
	if (cchar *cp{} ; (rs = sip->strw(sp,sl,&cp)) >= 0) {
	    rs = ulp->add(cp) ;
	}
	return rs ;
    } /* end subroutine (si_loadname) */
}

namespace ucent {
    int si_copystr(SI *sip,char **pp,cchar *sp) noex {
	int		rs = SR_OK ;
	cchar		**cpp = ccharpp(pp) ;
	*cpp = nullptr ;
	if (sp) {
	    rs = sip->strw(sp,-1,cpp) ;
	}
	return rs ;
    } /* end subroutine (si_copystr) */
}



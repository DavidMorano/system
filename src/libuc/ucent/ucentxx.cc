/* ucentxx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

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
	These are utility subroutines for the UCENT subroutine group
	within LIBUC library.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD |nothrow(3c++)| */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<storeitem.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<vechand.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<six.h>			/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ucentxx.hh"


/* local defines */

#define	SI	storeitem


/* imported namespaces */


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
	if ((rs = sbp->getlen) >= 0) ylikely {
	    cint	idx = rs ;
	    cint	nb = (idx >= clen) ? 1 : (clen-idx) ;
	    if ((rs = sbp->blanks(nb)) >= 0) {
		rs = sbp->str(sp) ;
	    }
	} /* end if */
	return rs ;
    } /* end subroutine (sbuf_fmtstr) */
} /* end namespace (ucent) */
#endif /* COMMENT (currently unused) */

namespace ucent {
    int si_loadnames(SI *sip,vechand *ulp,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	for (cchar *tp ; (tp = strnbrk(sp,sl," ,")) != nullptr ; ) {
	    if (cint tl = intconv(tp - sp) ; tl > 0) {
		c += 1 ;
		rs = si_loadname(sip,ulp,sp,tl) ;
	    } /* end if (non-zero) */
	    sl -= intconv((tp + 1) - sp) ;
	    sp = (tp + 1) ;
	    if (rs < 0) break ;
	} /* end for */
	if ((rs >= 0) && sl && sp[0]) {
	    c += 1 ;
	    rs = si_loadname(sip,ulp,sp,sl) ;
	} /* end if */
	return (rs >= 0) ? c : rs ;
    } /* end subroutine (si_loadnames) */
} /* end namespace (ucent) */

namespace ucent {
    int si_loadname(SI *sip,vechand *ulp,cchar *sp,int sl) noex {
	int		rs ;
	if (cchar *cp{} ; (rs = sip->strw(sp,sl,&cp)) >= 0) {
	    rs = ulp->add(cp) ;
	}
	return rs ;
    } /* end subroutine (si_loadname) */
} /* end namespace (ucent) */

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
} /* end namespace (ucent) */



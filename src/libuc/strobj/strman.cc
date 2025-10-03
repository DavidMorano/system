/* strman SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* special string manipulations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.  

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	strman

	Description:
        This object allows for some specialized manipulations on a
        counted-string object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<six.h>
#include	<char.h>		/* |char_iswhite(3uc)| */
#include	<mkchar.h>
#include	<localmisc.h>

#include	"strman.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct whitestop {
	strman	*sop ;
	int	sch ;
	whitestop(strman *p,int c) noex : sop(p), sch(c) { } ;
	operator bool () noex {
	    cint	ch = mkchar(sop->sp[0]) ;
	    return CHAR_ISWHITE(ch) && (ch != sch) ;
	} ;
    } ; /* end struct (whitestop) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int strman_start(strman *sop,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (sop && sp) ylikely {
	    rs = SR_OK ;
	    sop->sp = sp ;
	    sop->sl = sl ;
	}
	return rs ;
}
/* end subroutine (strman_start) */

int strman_finish(strman *sop) noex {
	int		rs = SR_FAULT ;
	if (sop) ylikely {
	    rs = sop->sl ;
	    sop->sp = nullptr ;
	    sop->sl = 0 ;
	}
	return rs ;
}
/* end subroutine (strman_finish) */

int strman_breakfield(strman *sop,cchar *ss,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (sop && ss && rpp) {
	    int		si = -1 ;
	    rs = SR_OK ;
	    *rpp = nullptr ;
	    if (sop->sl > 0) {
	        if ((si = siskipwhite(sop->sp,sop->sl)) > 0) {
	            sop->sp += si ;
	            sop->sl -= si ;
	        }
	        if (sop->sl > 0) {
	            *rpp = sop->sp ;
	            if ((si = sibrk(sop->sp,sop->sl,ss)) >= 0) {
	                rl = si ;
	                sop->sp += (si + 1) ;
	                sop->sl -= (si + 1) ;
	            } else {
	                rl = sop->sl ;
	                sop->sl = 0 ;
	            }
	        } /* end if (greater-than-zero) */
	    } /* end if (greater-than-zero) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (strman_breakfield) */

/* skip white-space until reach terminator character */
int strman_whitechr(strman *sop,int sch) noex {
    	int		rs = SR_FAULT ;
	int		sl = 0 ; /* return-value */
	if (sop) {
	    rs = SR_OK ;
	    for (whitestop pred(sop,sch) ; (sop->sl > 0) && pred ; ) {
	        sop->sp += 1 ;
	        sop->sl -= 1 ;
	    } /* end for */
	    sl = sop->sl ;
	} /* end if (non-null) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (strman_whitecolon) */

int strman_span(strman *sop,cchar *stuff) noex {
    	int		rs = SR_FAULT ;
	int		sl = 0 ; /* return-value */
	if (sop && stuff) {
	    rs = SR_OK ;
	    if (int si ; (si = sispan(sop->sp,sop->sl,stuff)) > 0) {
	        sop->sp += si ;
	        sop->sl -= si ;
	    }
	    sl = sop->sl ;
	} /* end if (non-null) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (strman_span) */



/* manstr SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* special string manipulations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.  

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	manstr

	Description:
        This object allows for some specialized manipulations on a
        counted-string object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<six.h>
#include	<char.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"manstr.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct whitestop {
	manstr	*sop ;
	int	sch ;
	whitestop(manstr *p,int c) noex : sop(p), sch(c) { } ;
	operator bool () noex {
	    cint	ch = mkchar(sop->sp[0]) ;
	    return CHAR_ISWHITE(ch) && (ch != sch) ;
	} ;
    } ; /* end struct (whitestop) */
}


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int manstr_start(manstr *sop,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (sop && sp) {
	    rs = SR_OK ;
	    sop->sp = sp ;
	    sop->sl = sl ;
	}
	return rs ;
}
/* end subroutine (manstr_start) */

int manstr_finish(manstr *sop) noex {
	int		rs = SR_FAULT ;
	if (sop) {
	    rs = sop->sl ;
	    sop->sp = nullptr ;
	    sop->sl = 0 ;
	}
	return rs ;
}
/* end subroutine (manstr_finish) */

int manstr_breakfield(manstr *sop,cchar *ss,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		rl = -1 ;
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
	            if ((si = sibreak(sop->sp,sop->sl,ss)) >= 0) {
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
/* end subroutine (manstr_breakfield) */

/* skip white-space until reach terminator character */
int manstr_whitechr(manstr *sop,int sch) noex {
	whitestop	pred(sop,sch) ;
	while ((sop->sl > 0) && pred) {
	    sop->sp += 1 ;
	    sop->sl -= 1 ;
	}
	return sop->sl ;
}
/* end subroutine (manstr_whitecolon) */

int manstr_span(manstr *sop,cchar *stuff) noex {
	int		si ;
	if ((si = sispan(sop->sp,sop->sl,stuff)) > 0) {
	    sop->sp += si ;
	    sop->sl -= si ;
	}
	return sop->sl ;
}
/* end subroutine (manstr_span) */



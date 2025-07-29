/* findinline SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find a TeX-type in-line text escape */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was adapted from another subroutine that did
	a similar type of function.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	findinline

	Description:
	This subroutine finds a TeX-like in-line text escape.

	Synopsis:
	int findinline_esc(findinline *fip,cchar *lp,int ll) noex

	Arguments:
	fip	pinter to FINDINLINE structure
	lp	supplied string to test
	ll	length of supplied string to test

	Returns:
	>=0	length of result "thing" 
	<0	error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ascii.h>
#include	<sfx.h>			/* |sfshrink(3uc)| */
#include	<strn.h>		/* |strnchr(3uc)| */
#include	<strwcpy.h>
#include	<mkchar.h>
#include	<ischarx.h>		/* |isalphalatin(3uc)| */
#include	<localmisc.h>

#include	"findinline.h"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	getdash(findinline *,cchar *,int) noex ;
static int	getpair(findinline *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int findinline_esc(findinline *fip,cchar *lp,int ll) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		skiplen = 0 ;
	if (fip && lp) ylikely {
	    rs = SR_OK ;
	    if (ll < 0) ll = lenstr(lp) ;
	    if (ll >= 3) ylikely {
	        for (cc *tp ; (tp = strnchr(lp,ll,CH_BSLASH)) != np ; ) {
		    cchar	*cp = (tp + 1) ;
		    fip->sp = tp ;
		    if (int cl = intconv((lp + ll) - (tp + 1)) ; cl > 0) {
		        int	ch = mkchar(cp[0]) ;
		        if (ch == '_') {
	        	    skiplen = getdash(fip,cp,cl) ;
		        } else if (isalphalatin(ch)) {
	        	    skiplen = getpair(fip,cp,cl) ;
		        }
	      	        if (skiplen > 0) break ;
		    } /* end if */
	            lp += 1 ;
	            ll -= 1 ;
	        } /* end while */
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? skiplen : rs ;
}
/* end subroutine (findinline) */


/* local subroutines */

static int getdash(findinline *fip,cchar *sp,int sl) noex {
	int		skiplen = 0 ;
	cchar		*start = (sp - 1) ;
	fip->kp = sp ;
	fip->kl = 1 ;
	sp += 1 ;
	sl -= 1 ;
	if (sl > 0) ylikely {
	    int		vl = 0 ;
	    fip->vp = sp ;
	    for (bool f ; sl ; ) {
		cint	ch = mkchar(sp[0]) ;
		f = isalphalatin(ch) ;
		if (!f) break ;
		sp += 1 ;
		sl -= 1 ;
		vl += 1 ;
	    } /* end for */
	    if (vl > 0) {
		fip->vl = vl ;
		skiplen = intconv(sp - start) ;
	    }
	} /* end if */
	return skiplen ;
}
/* end subroutine (getdash) */

static int getpair(findinline *fip,cchar *sp,int sl) noex {
    	cnullptr	np{} ;
	int		skiplen = 0 ;
	cchar		*start = (sp - 1) ;
	fip->kp = sp ;
	if (cchar *tp ; (tp = strnchr(sp,sl,CH_LBRACE)) != np) {
	    cint tl = intconv(tp - sp) ;
	    if (int kl ; (kl = sfshrink(sp,tl,nullptr)) > 0) {
		fip->kl = kl ;
		sl -= intconv((tp + 1) - sp) ;
		sp = (tp + 1) ;
		fip->vp = sp ;
	        if ((tp = strnchr(sp,sl,CH_RBRACE)) != nullptr) {
		    if ((tp-sp) > 0) {
			fip->vl = intconv(tp - sp) ;
			skiplen = intconv((tp + 1) - start) ;
		    }
		}
	    }
	} /* end if */
	return skiplen ;
}
/* end subroutine (getpair) */



/* findinline SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find a TeX-type in-line text escape */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was adapted from others programs that did
	similar types of functions.

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
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<estrings.h>
#include	<ascii.h>
#include	<strn.h>
#include	<strwcpy.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"findinline.h"


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
	int		rs = SR_FAULT ;
	int		skiplen = 0 ;
	if (fip && lp) {
	    rs = SR_OK ;
	    if (ll < 0) ll = strlen(lp) ;
	    if (ll >= 3) {
	        cchar	*tp, *cp ;
	        int	cl ;
	        while ((tp = strnchr(lp,ll,CH_BSLASH)) != nullptr) {
		    fip->sp = tp ;
		    cp = (tp+1) ;
		    cl = ((lp+ll)-(tp+1)) ;
		    if (cl > 0) {
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
	cchar		*start = (sp-1) ;
	fip->kp = sp ;
	fip->kl = 1 ;
	sp += 1 ;
	sl -= 1 ;
	if (sl > 0) {
	    int		vl = 0 ;
	    int		f ;
	    fip->vp = sp ;
	    while (sl) {
		cint	ch = mkchar(sp[0]) ;
		f = isalphalatin(ch) ;
		if (!f) break ;
		sp += 1 ;
		sl -= 1 ;
		vl += 1 ;
	    } /* end while */
	    if (vl > 0) {
		fip->vl = vl ;
		skiplen = (sp-start) ;
	    }
	} /* end if */
	return skiplen ;
}
/* end subroutine (getdash) */

static int getpair(findinline *fip,cchar *sp,int sl) noex {
	int		skiplen = 0 ;
	cchar		*start = (sp-1) ;
	cchar		*tp ;
	fip->kp = sp ;
	if ((tp = strnchr(sp,sl,CH_LBRACE)) != nullptr) {
	    int		kl ;
	    if ((kl = sfshrink(sp,(tp-sp),nullptr)) > 0) {
		fip->kl = kl ;
		sl -= ((tp+1)-sp) ;
		sp = (tp+1) ;
		fip->vp = sp ;
	        if ((tp = strnchr(sp,sl,CH_RBRACE)) != nullptr) {
		    if ((tp-sp) > 0) {
			fip->vl = (tp-sp) ;
			skiplen = ((tp+1)-start) ;
		    }
		}
	    }
	} /* end if */
	return skiplen ;
}
/* end subroutine (getpair) */



/* vecstrx_adduniqs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* add string(s) to a vector-string object */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-06-02, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	vecstrx_adduniqs

	Description:
	We add strings to the list, but we only add a given string
	once. So all of the strings in the list should end up being
	unique.

	Synopsis:
	int vecstrx_adduniqs(vecstrx *vlp,cchar *sp,int sl) noex

	Arguments:
	vlp		pointer to object
	sp		source string pointer
	sl		source string length

	Returns:
	>=0		number of strings entered
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<estrings.h>
#include	<localmisc.h>

#include	"vecstrx.hh"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int vecstrx::adduniqs(cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (sp) {
	    int		cl ;
	    cchar	*cp ;
	    cchar	*tp ;
	    rs = SR_OK ;
	    if (sl < 0) sl = xstrlen(sp) ;
	    while ((tp = strnbrk(sp,sl," ,")) != nullptr) {
		cint	tl = intconv(tp - sp) ;
	        if ((cl = sfshrink(sp,tl,&cp)) > 0) {
	            rs = adduniq(cp,cl) ;
	            c += (rs < INT_MAX) ;
	        }
	        sl -= intconv((tp + 1) - sp) ;
	        sp = (tp + 1) ;
	        if (rs < 0) break ;
	    } /* end while */
	    if ((rs >= 0) && (sl > 0)) {
	        if ((cl = sfshrink(sp,sl,&cp)) > 0) {
	            rs = adduniq(cp,cl) ;
	            c += (rs < INT_MAX) ;
	        }
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstrx_adduniqs) */



/* siletter SUPPORT */
/* lang=C++20 */

/* is the ... something ... a "letter" (whatever that is)? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A�D� Morano
	This object module was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	We check for some condition.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<usystem.h>
#include	<mkchar.h>
#include	<sfx.h>
#include	<strn.h>
#include	<six.h>
#include	<hasx.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"siletter.h"


/* local defines */


/* exported variables */


/* exported subroutines */

int siletter(siletter_res *lsp,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		si = 0 ;
	if (lsp && sp) {
	    memclear(lsp) ;		/* potentially dangerous */
	    if (sl > 0) {
	        if (cchar *tp ; (tp = strnchr(sp,sl,'.')) != nullptr) {
	            cchar	*cp{} ;
	            if (int cl ; (cl = sfshrink(sp,(tp - sp),&cp)) > 0) {
	                if (hasalluc(cp,cl)) {
	                    lsp->lp = cp ;
	                    lsp->ll = cl ;
	                    si = ((tp + 1) - sp) ;
		        }
	            } /* end if (shrink) */
	        } /* end if (had a period) */
	    } /* end if (positive) */
	} /* end if (non-null) */
	return (rs >= 0) ? si : rs ;
}
/* end subroutine (siletter) */



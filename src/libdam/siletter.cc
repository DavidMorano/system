/* siletter SUPPORT (String-Index-Letter) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* String-Index-Letter string searching function */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	siletter

	Description:
	This is some sort of special String-Index searching function.
	It is used in parsing certain kinds of texts.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<usystem.h>
#include	<sfx.h>
#include	<strn.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"siletter.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int siletter(siletter_res *lsp,cchar *sp,int sl) noex {
	SILETTER_RES	*hop = lsp ;
	int		rs = SR_FAULT ;
	int		si = 0 ;
	if (lsp && sp) {
	    memclear(hop) ;
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



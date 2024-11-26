/* bvcitekey SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* manage BV cite key */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We manage a BV cite-key object.

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"bvcitekey.h"


/* local defines */


/* external subroutines */


/* external variables */


/* exported variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int bvcitekey_set(bvcitekey *bvp,uint *ckp) noex {
	int		rs = SR_FAULT ;
	if (bvp && ckp) {
	    uint	ck = 0 ;
	    ck |= (bvp->nlines & UCHAR_MAX) ;
	    ck = (ck << 8) ;
	    ck |= (bvp->b & UCHAR_MAX) ;
	    ck = (ck << 8) ;
	    ck |= (bvp->c & UCHAR_MAX) ;
	    ck = (ck << 8) ;
	    ck |= (bvp->v & UCHAR_MAX) ;
	    *ckp = ck ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bvcitekey_set) */

int bvcitekey_get(bvcitekey *bvp,uint *ckp) noex {
	int		rs = SR_FAULT ;
	if (bvp && ckp) {
	    uint	ck = *ckp ;
	    bvp->v = (ck & UCHAR_MAX) ;
	    ck = (ck >> 8) ;
	    bvp->c = (ck & UCHAR_MAX) ;
	    ck = (ck >> 8) ;
	    bvp->b = (ck & UCHAR_MAX) ;
	    ck = (ck >> 8) ;
	    bvp->nlines = (ck & UCHAR_MAX) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bvcitekey_get) */



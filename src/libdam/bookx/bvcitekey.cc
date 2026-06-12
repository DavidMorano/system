/* bvcitekey SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* manage BV cite key */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	bvcitekey

	Description:
	We manage a BV cite-key object.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

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
	    rs = SR_OK  ;
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
	    rs = SR_OK  ;
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



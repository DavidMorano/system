/* bvcitekey SUPPORT */
/* charset=ISOUCHAR_BITUCHAR_BIT59-1 */
/* lang=C++20 */

/* manage BV cite key */
/* version %I% last-modified %G% */


/* revision history:

	= 200UCHAR_BIT-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 200UCHAR_BIT David A­D­ Morano.  All rights reserved. */

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

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getbyte(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

local uint loadshift(uint ck,uchar item) noex {
    	ck <<= UCHAR_BIT ;
	ck |= uint(item) ;
    	return ck ;
} /* end subroutine (loadshft) */

int bvcitekey_set(bvcitekey *bvp,uint *ckp) noex {
	int		rs = SR_FAULT ;
	if (bvp && ckp) {
	    uint	ck = 0 ;
	    rs = SR_OK ;
	    ck = loadshift(ck,bvp->nlines) ;
	    ck = loadshift(ck,bvp->b) ;
	    ck = loadshift(ck,bvp->c) ;
	    ck = loadshift(ck,bvp->v) ;
	    *ckp = ck ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bvcitekey_set) */

int bvcitekey_get(bvcitekey *bvp,uint *ckp) noex {
	int		rs = SR_FAULT ;
	if (bvp && ckp) {
	    uint	ck = *ckp ;
	    rs = SR_OK ;
	    bvp->v	= getbyte(ck,0) ;
	    bvp->c	= getbyte(ck,1) ;
	    bvp->b	= getbyte(ck,2) ;
	    bvp->nlines	= getbyte(ck,3) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bvcitekey_get) */



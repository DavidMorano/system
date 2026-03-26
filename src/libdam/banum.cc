/* banum SUPPORT (BitNumarr) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* perform some bit-array type operations */
/* version %I% last-modified %G% */


/* revistion history:

	= 1998-05-27, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	banum

	Description:
	This module does some bit-array type stuff.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* |INT_MAX| + |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<uclibmem.h>
#include	<intsat.h>
#include	<localmisc.h>

#include	"banum.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#ifndef	BANUM_DIGIT
#ifndef	BANUM_DIGIT	int
#endif

#define	BANUM_LIM16	(1 << 16)
#define	BANUM_BPW	(szof(BANUM_DIGIT) * CHAR_BIT)


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libuc::libmem ;			/* variable */


/* local typedefs */

typedef BANUM_DIGIT *	digitp ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	numbits(int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int banum_start(ba *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->num = nullptr ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (banum_start) */

int banum_finish(ba *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->num) {
	        rs1 = libmem.free(op->num) ;
	        if (rs >= 0) rs = rs1 ;
	        op->num = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (banum_finish) */

int banum_prepare(banum *cnp) noex {
	cint		asz = (BANUM_LIM16 * szof(int)) ;
	int		rs = SR_FAULT ;
	if (cnp) {
	    if (void *vp ; (rs = libmem.mall(asz,&vp)) >= 0) {
	        cnp->num = intp(vp) ;
	        for (int i = 0 ; i < BANUM_LIM16 ; i += 1) {
	            cnp->num[i] = numbits(i) ;
	        }
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (banum_prepare) */

int banum_num(banum *op) noex {
	int		sum = 0 ;
	int		rs = SR_FAULT ;
	if (op) {
	    int		*na = op ;
	    rs = SR_OK ;
	    for (int i = 0 ; i < op->nwords ; i += 1) {
	        BANUM_DIGIT	v = op->a[i] ;
	        sum += na[v & (BANUM_LIM16 - 1)] ; v >>= 16 ;
	        sum += na[v & (BANUM_LIM16 - 1)] ; v >>= 16 ;
	        sum += na[v & (BANUM_LIM16 - 1)] ; v >>= 16 ;
	        sum += na[v & (BANUM_LIM16 - 1)] ; v >>= 16 ;
	    } /* end for */
	    rs = intsat(sum) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (banum_num) */

int banum_forsake(banum *cnp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (cnp) {
	    rs = SR_OK ;
	    if (cnp->num) {
	        rs1 = libmem.free(cnp->num) ;
	        if (rs >= 0) rs = rs1 ;
	        cnp->num = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (banum_forsake) */


/* private subroutines */

local int numbits(int n) noex {
	int		sum = 0 ;
	while (n) {
	    if (n & 1) sum += 1 ;
	    n = n >> 1 ;
	} /* end while */
	return sum ;
}
/* end subroutine (numbits) */



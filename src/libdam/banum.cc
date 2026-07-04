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
#include	<climits>		/* CSTD |INT_MAX| + |CHAR_BIT| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<intsat.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"banum.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#ifndef	BANUM_DIGIT
#define	BANUM_DIGIT	int
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

int banum_start(banum *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->num = nullptr ;
	    op->nwords = 0 ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (banum_start) */

int banum_finish(banum *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->num) {
	        rs1 = libmem.free(op->num) ;
	        if (rs >= 0) rs = rs1 ;
	        op->num = nullptr ;
	    } /* end if (memory-release) */
	    op->nwords = 0 ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (banum_finish) */

int banum_prepare(banum *op) noex {
	cint		asz = (BANUM_LIM16 * szof(BANUM_DIGIT)) ;
	int		rs = SR_FAULT ;
	if (op) {
	    cint n = BANUM_LIM16 ;
	    if (void *vp ; (rs = libmem.mall(asz,&vp)) >= 0) {
	        op->num = digitp(vp) ;
	        for (int i = 0 ; i < n ; i += 1) {
	            op->num[i] = numbits(i) ;
	        } /* end for */
		op->nwords = n ;
	    } /* end if (memory-acquire) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (banum_prepare) */

#ifdef	COMMENT
int banum_num(banum *op) noex {
	int		sum = 0 ;
	int		rs = SR_FAULT ;
	if (op) {
	    int		*na = op ;
	    rs = SR_OK ;
	    for (int i = 0 ; i < op->nwords ; i += 1) {
	        BANUM_DIGIT	v = op->num[i] ;
	        sum += na[v & (BANUM_LIM16 - 1)] ; v >>= 16 ;
	        sum += na[v & (BANUM_LIM16 - 1)] ; v >>= 16 ;
	        sum += na[v & (BANUM_LIM16 - 1)] ; v >>= 16 ;
	        sum += na[v & (BANUM_LIM16 - 1)] ; v >>= 16 ;
	    } /* end for */
	    rs = intsat(sum) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (banum_num) */
#endif /* COMMENT */

int banum_forsake(banum *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->num) {
	        rs1 = libmem.free(op->num) ;
	        if (rs >= 0) rs = rs1 ;
	        op->num = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (banum_forsake) */


/* private subroutines */

local int numbits(int n) noex {
	int		sum = 0 ;
	while (n) {
	    if (n & 1) sum += 1 ;
	    n >>= 1 ;
	} /* end while */
	return sum ;
} /* end subroutine (numbits) */



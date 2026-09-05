/* ba SUPPORT (BitArray) */
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
	ba

	Description:
	This module does some bit-array type stuff.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* CSTD |INT_MAX| + |CHAR_BIT| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<bit>			/* C++SYD |popcount(2c++)| */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ba.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |memclear(3u)| */
import varithmetic ;			/* |vlshr| + |vlshl| */

/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::popcount ;			/* subroutine */
using libuc::libmem ;			/* variable */


/* local typedefs */

typedef BA_DIGIT *	digitp ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int ba_start(ba *op,int n) noex {
	cint		wsz = szof(BA_DIGIT) ;
	int		rs = SR_FAULT ;
	if (op) {
    	    cint	bpw = (wsz * CHAR_BIT) ;
	    rs = SR_INVALID ;
	    if (n >= 0) {
	        cint 	nw = ((n / bpw) + 1) ;
	        if (void *vp ; (rs = libmem.call(nw,wsz,&vp)) >= 0) {
	            op->a = digitp(vp) ;
	            op->nbits = n ;
	            op->nwords = nw ;
	        } /* end if (memory-acquire) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ba_start) */

int ba_finish(ba *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->a) {
	        rs1 = libmem.free(op->a) ;
	        if (rs >= 0) rs = rs1 ;
	        op->a = nullptr ;
	    } /* end if (memory-release) */
	    op->nbits = 0 ;
	    op->nwords = 0 ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ba_finish) */

int ba_setones(ba *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    int		asz = op->nwords * szof(BA_DIGIT) ;
	    rs = SR_OK ;
	    memnset(op->a,esz,UCHAR_MAX,asz) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ba_setones) */

int ba_zero(ba *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    cint	asz = op->nwords * szof(BA_DIGIT) ;
	    rs = memclear(op->a,asz) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ba_zero) */

int ba_countdown(ba *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    int		r = 0 ;
	    bool	f_borrow ;
	    bool	f_msb1 ;
	    bool	f_msb2 ;
	    rs = SR_OK ;
	    do {
	        f_msb1 = !!(op->a[r] & INT_MAX) ;
	        op->a[r] -= 1 ;
	        f_msb2 = !!(op->a[r] & INT_MAX) ;
	        f_borrow = (! f_msb1) && f_msb2 ;
	        r += 1 ;
	    } while (f_borrow && (r < op->nwords)) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ba_countdown) */

int ba_and(ba *op1,ba *op2) noex {
	int		rs = SR_FAULT ;
	if (op1 && op2) {
	    cint	nw = min(op1->nwords,op2->nwords) ;
	    rs = SR_OK ;
	    for (int i = 0 ; i < nw ; i += 1) {
	        op1->a[i] = op1->a[i] & op2->a[i] ;
	    } /* end for */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ba_and) */

int ba_numones(ba *op) noex {
	int		rs = SR_FAULT ;
	int		sum = 0 ; /* return-value */
	if (op) {
	    rs = SR_OK ;
	    for (int i = 0 ; i < op->nwords ; i += 1) {
	        sum += popcount(op->a[i]) ;
	    } /* end for */
	    rs = sum ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ba_numones) */

local int ba_shx(ba *op,int nbits,bool f) noex {
    	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_INVALID ;
	    if ((nbits >= 0) && (nbits < op->nbits)) {
		rs = SR_OK ;
		if (nbits > 0) {
	            cint n	= op->nwords ;
	            cint sz	= op->nwords * szof(BA_DIGIT) ;
	            if (BA_DIGIT *tmp ; (rs = libmem.mall(sz,&tmp)) >= 0) {
		        {
			    if (f) {
		                vlshl(n,tmp,op->a,nbits) ;
			    } else {
		                vlshr(n,tmp,op->a,nbits) ;
			    }
		            memcopy(op->a,tmp,sz) ;
		        }
		        rs1 = libmem.free(tmp) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (m-a-f) */
	        } /* end if (needed) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ba_shx) */

int ba_shr(ba *op,int nbits) noex {
    	return ba_shx(op,nbits,false) ;
} /* end subroutine (ba_shr) */

int ba_shl(ba *op,int nbits) noex {
    	return ba_shx(op,nbits,true) ;
} /* end subroutine (ba_shl) */


/* private subroutines */



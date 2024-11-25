/* ba SUPPORT (BitArray) */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* perform some bit-array type operations */
/* version %I% last-modified %G% */


/* revistion history:

	= 1998-05-27, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	ba

	Description:
	This module does some bit-array type stuff.

	Notes:
	a) we dynamically create a look-up table using | banum_prepare()|

*******************************************************************************/

#include	<envstandards.h>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<localmisc.h>

#include	"ba.h"


/* local defines */

#define	BA_MAX16	(1 << 16)
#define	BA_BITSPERWORD	(8 * sizeof(ulong))


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef ba_num *	nump ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	numbits(int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ba_start(ba *op,ba_num *cnp,int n) noex {
	cint		nw = ((n / BA_BITSPERWORD) + 1) ;
	int		rs = SR_FAULT ;
	if (op) {
	    void	*vp{} ;
	    op->cnp = nullptr ;
	    cint	hsz = nw * sizeof(ulong) ;
	    if ((rs = uc_malloc(hsz,&vp)) >= 0) {
	        op->a = ulongp(vp) ;
	        for (int i = 0 ; i < nw ; i += 1) {
		    op->a[i] = 0 ;
		}
	        op->cnp = cnp ;
	        op->nbits = n ;
	        op->nwords = nw ;
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ba_start) */

int ba_finish(ba *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->a) {
	        rs1 = uc_free(op->a) ;
	        if (rs >= 0) rs = rs1 ;
	        op->a = nullptr ;
	    }
	    op->cnp = nullptr ;
	    op->nbits = 0 ;
	    op->nwords = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ba_finish) */

int ba_setones(ba *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    int		asz = op->nwords * sizeof(ulong) ;
	    rs = SR_OK ;
	    memset(op->a,(~0),asz) ;
	}
	return rs ;
}
/* end subroutine (ba_setones) */

int ba_zero(ba *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    cint	asz = op->nwords * sizeof(ulong) ;
	    rs = memclear(op->a,asz) ;
	}
	return rs ;
}
/* end subroutine (ba_zero) */

int ba_countdown(ba *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    int		r = 0 ;
	    int		f_borrow ;
	    int		f_msb1, f_msb2 ;
	    rs = SR_OK ;
	    do {
	        f_msb1 = op->a[r] & INT_MAX ;
	        op->a[r] -= 1 ;
	        f_msb2 = op->a[r] & INT_MAX ;
	        f_borrow = (! f_msb1) && f_msb2 ;
	        r += 1 ;
	    } while (f_borrow && (r < op->nwords)) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ba_countdown) */

int ba_and(ba *op1,ba *op2) noex {
	int		rs = SR_FAULT ;
	if (op1 && op2) {
	    cint	nw = min(op1->nwords,op2->nwords) ;
	    rs = SR_OK ;
	    for (int i = 0 ; i < nw ; i += 1) {
	        op1->a[i] = op1->a[i] & op2->a[i] ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ba_and) */

int ba_numones(ba *op) noex {
	ulong		sum = 0 ;
	int		rs = SR_FAULT ;
	if (op) {
	    int		*na = (op->cnp)->num ;
	    rs = SR_OK ;
	    for (int i = 0 ; i < op->nwords ; i += 1) {
	        ulong	v = op->a[i] ;
	        sum += na[v & (BA_MAX16 - 1)] ; v >>= 16 ;
	        sum += na[v & (BA_MAX16 - 1)] ; v >>= 16 ;
	        sum += na[v & (BA_MAX16 - 1)] ; v >>= 16 ;
	        sum += na[v & (BA_MAX16 - 1)] ; v >>= 16 ;
	    } /* end for */
	    rs = int(sum) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ba_numones) */


/* other interfaces */

int banum_prepare(ba_num *cnp) noex {
	cint		asz = (BA_MAX16 * sizeof(int)) ;
	int		rs = SR_FAULT ;
	if (cnp) {
	    void	*vp{} ;
	    if ((rs = uc_malloc(asz,&vp)) >= 0) {
	        cnp->num = intp(vp) ;
	        for (int i = 0 ; i < BA_MAX16 ; i += 1) {
	            cnp->num[i] = numbits(i) ;
	        }
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (banum_prepare) */

int banum_forsake(ba_num *cnp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (cnp) {
	    rs = SR_OK ;
	    if (cnp->num) {
	        rs1 = uc_free(cnp->num) ;
	        if (rs >= 0) rs = rs1 ;
	        cnp->num = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (banum_forsake) */


/* private subroutines */

static int numbits(int n) noex {
	int		sum = 0 ;
	while (n) {
	    if (n & 1) sum += 1 ;
	    n = n >> 1 ;
	}
	return sum ;
}
/* end subroutine (numbits) */



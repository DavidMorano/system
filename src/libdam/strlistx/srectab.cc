/* srectab SUPPORT (String-Record-Table) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* record-table */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	srectab

	Description:
	Record-Table.

	Synopsis:

	Arguments:

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>		/* CSTD |UINT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"srectab.h"


/* local defines */

#define	SRECTAB_DEFENTS		10


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* exported variables */


/* local structures */


/* forward references */


/* local variables */

cint		dents = SRECTAB_DEFENTS ;


/* exported variables */


/* exported subroutines */

int srectab_start(srectab *rtp,int n) noex {
	int		rs = SR_FAULT ;
	int		sz ;
	if (n < dents) n = dents ;
	if (rtp) ylikely {
	    rtp->i = 0 ;
	    rtp->n = n ;
	    sz = (n + 1) * szof(int) ;
	    if (void *vp ; (rs = lm_mall(sz,&vp)) >= 0) {
	        rtp->rt = uintp(vp) ;
	        rtp->rt[0] = 0 ;
	        rtp->i = 1 ;
	    } /* end if (memory-acquire) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (srectab_start) */

int srectab_finish(srectab *rtp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (rtp) ylikely {
	    rs = SR_OK ;
	    if (rtp->rt) {
	        rs1 = lm_free(rtp->rt) ;
	        if (rs >= 0) rs = rs1 ;
	        rtp->rt = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (srectab_finish) */

int srectab_add(srectab *rtp,uint ki) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ; /* return-value */
	if (rtp) ylikely {
	    i = rtp->i ; 
	    rs = SR_OK ;
	    if ((i + 1) > rtp->n) {
	        rs = srectab_extend(rtp) ;
	    }
	    if (rs >= 0) ylikely {
	        rtp->rt[i] = ki ;
	        rtp->i += 1 ;
	    } /* end if (ok) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (srectab_add) */

int srectab_extend(srectab *rtp) noex {
	int		nn = (rtp->n * 2) ;
	int		rs = SR_OK ;
	if ((rtp->i + 1) > rtp->n) {
	    int		sz = (nn + 1) * szof(int) ;
	    if (uint *va ; (rs = lm_rall(rtp->rt,sz,&va)) >= 0) {
	        rtp->rt = va ;
	        rtp->n = nn ;
	    } /* end if (memory-reallocation) */
	} /* end if */
	return rs ;
} /* end subroutine (srectab_extend) */

int srectab_done(srectab *rtp) noex {
    	int		rs = SR_FAULT ;
	int		i = 0 ; /* return-value */
	if (rtp) ylikely {
	    rs = SR_OK ;
	    i = rtp->i ;
	    rtp->rt[i] = UINT_MAX ;
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (srectab_done) */

int srectab_count(srectab *rtp) noex {
    	int		rs = SR_FAULT ;
	int		i = 0 ; /* return-value */
	if (rtp) ylikely {
	    rs = SR_OK ;
	    i = rtp->i ;
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (srectab_count) */

int srectab_getvec(srectab *rtp,uint **rpp) noex {
    	int		rs = SR_FAULT ;
	int		i = 0 ; /* return-value */
	if (rtp) ylikely {
	    rs = SR_OK ;
	    if (rpp) *rpp = rtp->rt ;
	    i = rtp->i ;
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (srectab_getvec) */



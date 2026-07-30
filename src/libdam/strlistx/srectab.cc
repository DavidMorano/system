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


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* exported variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int srectab_start(srectab *rtp,int n) noex {
	int		rs = SR_OK ;
	int		sz ;
	if (n < 10) n = 10 ;
	rtp->i = 0 ;
	rtp->n = n ;
	sz = (n + 1) * szof(int) ;
	if (void *vp ; (rs = lm_mall(sz,&vp)) >= 0) {
	    rtp->rt = uintp(vp) ;
	    rtp->rt[0] = 0 ;
	    rtp->i = 1 ;
	} /* end if (memory-acquire) */
	return rs ;
} /* end subroutine (srectab_start) */

int srectab_finish(srectab *rtp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (rtp->rt) {
	    rs1 = lm_free(rtp->rt) ;
	    if (rs >= 0) rs = rs1 ;
	    rtp->rt = nullptr ;
	} /* end if (memory-release) */
	return rs ;
} /* end subroutine (srectab_finish) */

int srectab_add(srectab *rtp,uint ki) noex {
	int		rs = SR_OK ;
	int		i = rtp->i ;
	if ((i + 1) > rtp->n) {
	    rs = srectab_extend(rtp) ;
	}
	if (rs >= 0) {
	    rtp->rt[i] = ki ;
	    rtp->i += 1 ;
	} /* end if (ok) */
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
	int		i = rtp->i ;
	rtp->rt[i] = UINT_MAX ;
	return i ;
} /* end subroutine (srectab_done) */

int srectab_count(srectab *rtp) noex {
	return rtp->i ;
} /* end subroutine (srectab_count) */

int srectab_getvec(srectab *rtp,uint **rpp) noex {
	*rpp = rtp->rt ;
	return rtp->i ;
} /* end subroutine (srectab_getvec) */



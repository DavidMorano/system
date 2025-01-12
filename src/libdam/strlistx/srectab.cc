/* srectab SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* record-table */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
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
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<sysval.hh>
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<endian.h>
#include	<hash.h>
#include	<sfx.h>
#include	<sncpyx.h>
#include	<mkx.h>
#include	<isnot.h>
#include	<localmisc.h>		/* |MODP2(3dam)| */

#include	"srectab.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


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
	void		*vp ;
	if (n < 10) n = 10 ;
	rtp->i = 0 ;
	rtp->n = n ;
	sz = (n + 1) * szof(int) ;
	if ((rs = uc_malloc(sz,&vp)) >= 0) {
	    rtp->rt = uintp(vp) ;
	    rtp->rt[0] = 0 ;
	    rtp->i = 1 ;
	}
	return rs ;
}
/* end subroutine (srectab_start) */

int srectab_finish(srectab *rtp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (rtp->rt) {
	    rs1 = uc_free(rtp->rt) ;
	    if (rs >= 0) rs = rs1 ;
	    rtp->rt = nullptr ;
	}
	return rs ;
}
/* end subroutine (srectab_finish) */

int srectab_add(srectab *rtp,uint ki) noex {
	int		rs = SR_OK ;
	int		i = rtp->i ;
	if ((i + 1) > rtp->n) {
	    rs = srectab_extend(rtp) ;
	}
	if (rs >= 0) {
	    rtp->rt[i] = ki ;
	    rtp->i += 1 ;
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (srectab_add) */

int srectab_extend(srectab *rtp) noex {
	int		nn = (rtp->n * 2) ;
	int		rs = SR_OK ;
	if ((rtp->i + 1) > rtp->n) {
	    uint	*va{} ;
	    int		sz = (nn + 1) * szof(int) ;
	    if ((rs = uc_realloc(rtp->rt,sz,&va)) >= 0) {
	        rtp->rt = va ;
	        rtp->n = nn ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (srectab_extend) */

int srectab_done(srectab *rtp) noex {
	int		i = rtp->i ;
	rtp->rt[i] = UINT_MAX ;
	return i ;
}
/* end subroutine (srectab_done) */

int srectab_count(srectab *rtp) noex {
	return rtp->i ;
}
/* end subroutine (srectab_count) */

int srectab_getvec(srectab *rtp,uint **rpp) noex {
	*rpp = rtp->rt ;
	return rtp->i ;
}
/* end subroutine (srectab_getvec) */



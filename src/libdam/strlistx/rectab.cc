/* rectab SUPPORT */
/* lang=C++20 */

/* record-table */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	rectab

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

#include	"rectab.h"


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

int rectab_start(rectab *rtp,int n) noex {
	int		rs = SR_OK ;
	int		sz ;
	void		*vp ;
	if (n < 10) n = 10 ;
	rtp->i = 0 ;
	rtp->n = n ;
	sz = (n + 1) * sizeof(int) ;
	if ((rs = uc_malloc(sz,&vp)) >= 0) {
	    rtp->rt = uintp(vp) ;
	    rtp->rt[0] = 0 ;
	    rtp->i = 1 ;
	}
	return rs ;
}
/* end subroutine (rectab_start) */

int rectab_finish(rectab *rtp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (rtp->rt) {
	    rs1 = uc_free(rtp->rt) ;
	    if (rs >= 0) rs = rs1 ;
	    rtp->rt = nullptr ;
	}
	return rs ;
}
/* end subroutine (rectab_finish) */

int rectab_add(rectab *rtp,uint ki) noex {
	int		rs = SR_OK ;
	int		i = rtp->i ;
	if ((i + 1) > rtp->n) {
	    rs = rectab_extend(rtp) ;
	}
	if (rs >= 0) {
	    rtp->rt[i] = ki ;
	    rtp->i += 1 ;
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (rectab_add) */

int rectab_extend(rectab *rtp) noex {
	int		nn = (rtp->n * 2) ;
	int		rs = SR_OK ;
	if ((rtp->i + 1) > rtp->n) {
	    uint	*va{} ;
	    int		sz = (nn + 1) * sizeof(int) ;
	    if ((rs = uc_realloc(rtp->rt,sz,&va)) >= 0) {
	        rtp->rt = va ;
	        rtp->n = nn ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (rectab_extend) */

int rectab_done(rectab *rtp) noex {
	int		i = rtp->i ;
	rtp->rt[i] = UINT_MAX ;
	return i ;
}
/* end subroutine (rectab_done) */

int rectab_count(rectab *rtp) noex {
	return rtp->i ;
}
/* end subroutine (rectab_count) */

int rectab_getvec(rectab *rtp,uint **rpp) noex {
	*rpp = rtp->rt ;
	return rtp->i ;
}
/* end subroutine (rectab_getvec) */



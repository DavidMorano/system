/* requests SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C99 */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"requests.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int requests_start(requests *op) noex {
	cint		esize = sizeof(requests_it) ;
	cint		vn = 1000 ;
	cint		vo = (VECOBJ_OREUSE|VECOBJ_OSWAP) ;
	int		rs = SR_FAULT ;
	if (op) {
	    rs = vecobj_start(&op->ents,esize,vn,vo) ;
	}
	return rs ;
}
/* end subroutine (requests_start) */

int requests_finish(requests *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = vecobj_finish(&op->ents) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (requests_finish) */

int requests_get(requests *op,int i,requests_it *ip) noex {
	int		rs ;
	void		*vp{} ;
	memclear(ip) ;
	if ((rs = vecobj_get(&op->ents,i,&vp)) >= 0) {
	    requests_it		*ep = (requests_it *) vp ;
	    if (vp) {
	        *ip = *ep ;
		rs = ep->ro ;
	    } else {
		ip->ro = -1 ;
		ip->rs = 0 ;
		rs = INT_MAX ;
	    }
	} /* end if (vecobj_get) */
	return rs ;
}
/* end subroutine (requests_get) */

int requests_add(requests *op,requests_it *ip) noex {
	int		rs = SR_FAULT ;
	cvoid		*ep = (cvoid *) ip ;
	if (op) {
	    rs = vecobj_add(&op->ents,ep) ;
	}
	return rs ;
}
/* end subroutine (requests_store) */

int requests_del(requests *op,int i) noex {
	int		rs ;
	rs = vecobj_del(&op->ents,i) ;
	return rs ;
}
/* end subroutine (requests_del) */

int requests_count(requests *op) noex {
	int		rs ;
	rs = vecobj_count(&op->ents) ;
	return rs ;
}
/* end subroutine (requests_count) */



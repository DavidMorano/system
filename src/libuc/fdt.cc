/* fdt SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* File-Descriptor-Table */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<vecobj.h>
#include	<localmisc.h>

#include	"fdt.h"
#include	"ucb.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	fdt_get(FDT *,int,UCB **) noex ;
static int	fdt_free(FDT *,int) noex ;


/* local variables */

static FDT	fdt ;


/* exported variables */


/* private subroutines */

static int fdt_alloc(FDT *op,int fd,UCB *rpp) noex {
	FDT_ENT		*ep ;
	int		rs = SR_OK ;
	int		sz ;

	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != FDT_MAGIC) {

		sz = szof(FDT_ENT) ;
		rs = vecobj_start(&op->entries,sz,10,0) ;

		if (rs < 0)
		goto ret0 ;

	} /* end if (initialization) */

/* do we already have it? */

	while (fdt_getentry(op,fd,&ep) >= 0) {
			vecobj_del(&op->entries,0) ;
	}

	for (int i = 0 ; vecobj_get(&op->entries,i,&ep) >= 0 ; i += 1) {
	    if (ep) {
		if (ep->fd == fd) {


		}
	    }
	} /* end for */

/* allocate a new one */


ret0:
	return rs ;
}
/* end subroutine (fdt_alloc) */

static int fdt_getentry(FDT *op,int fd,FDT_ENT **rpp) noex {
	FDT_ENT		*ep = nullptr ;
	vecobj		*flp = &op->entries ;
	int		rs ;
	*rpp = nullptr ;
	for (int i = 0 ; (rs = flp->get(i,&ep)) >= 0 ; i += 1) {
	    if (ep) {
		if (ep->fd == fd) break ;
	    }
	} /* end for */
	if (rs >= 0) *rpp = ep ;
	return rs ;
}
/* end subroutine (fdt_getentry) */



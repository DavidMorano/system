/* ucenumsv SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* enumerate system SYSDB-XX entries */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was morphed from some previous one.  I
	do not remember what the previous one was.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	ucenumsv

	Description:
	We enumerate (reentrantly and thread safely) SYSDB entries
	from the system SYSDB-XX database.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<sysdbfn.h>
#include	<localmisc.h>

#include	"ucenumxx.h"
#include	"ucenumsv.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr uint		mxx = UCENUMSV_MAGIC ;
constexpr sysdbfiles	w = sysdbfile_sv ;


/* local subroutines */


/* exported variables */


/* exported subroutines */

int ucenumsv_open(ucenumsv *op,cchar *efname) noex {
	ucenumxxco<ucenumsv_ent>	ueo(op,mxx) ;
	int		rs ;
	if (cchar *rp ; (rs = sysdbfn_get(w,efname,&rp)) >= 0) {
	    rs = ueo.open(rp) ;
	} /* end if (sysdbfn_get) */
	return rs ;
}
/* end if (ucenumsv_open) */

int ucenumsv_close(ucenumsv *op) noex {
	ucenumxxco<ucenumsv_ent>	ueo(op,mxx) ;
	return ueo.close() ;
} 
/* end subroutine (ucenumsv_close) */

int ucenumsv_readent(ucenumsv *op,ucenumsv_ent *ep,char *bp,int bl) noex {
	ucenumxxco<ucenumsv_ent>	ueo(op,mxx) ;
	return ueo.readent(ep,bp,bl) ;
}
/* end subroutine (ucenumsv_readent) */

int ucenumsv_reset(ucenumsv *op) noex {
	ucenumxxco<ucenumsv_ent>	ueo(op,mxx) ;
	return ueo.reset() ;
}
/* end subroutine (ucenumsv_reset) */


/* local subroutines */

int ucenumsv::readent(ucenumsv_ent *uap,char *uabuf,int ualen) noex {
	return ucenumsv_readent(this,uap,uabuf,ualen) ;
}

void ucenumsv::dtor() noex {
	if (cint rs = close ; rs < 0) {
	    ulogerror("ucenumsv",rs,"fini-close") ;
	}
}

int ucenumsv_op::operator () (cchar *fn) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case ucenumsvmem_open:
	        rs = ucenumsv_open(op,fn) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (ucenumsv_op::operator) */

ucenumsv_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case ucenumsvmem_reset:
	        rs = ucenumsv_reset(op) ;
	        break ;
	    case ucenumsvmem_close:
	        rs = ucenumsv_close(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (ucenumsv_co::operator) */



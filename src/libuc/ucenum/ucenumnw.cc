/* ucenumnw SUPPORT */
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
	ucenumnw

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
#include	"ucenumnw.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr uint		mxx = UCENUMNW_MAGIC ;
constexpr sysdbfiles	w = sysdbfile_nw ;


/* local subroutines */


/* exported variables */


/* exported subroutines */

int ucenumnw_open(ucenumnw *op,cchar *efname) noex {
	ucenumxxco<ucenumnw_ent>	ueo(op,mxx) ;
	int		rs ;
	if (cchar *rp ; (rs = sysdbfn_get(w,efname,&rp)) >= 0) {
	    rs = ueo.open(rp) ;
	} /* end if (sysdbfn_get) */
	return rs ;
}
/* end if (ucenumnw_open) */

int ucenumnw_close(ucenumnw *op) noex {
	ucenumxxco<ucenumnw_ent>	ueo(op,mxx) ;
	return ueo.close() ;
} 
/* end subroutine (ucenumnw_close) */

int ucenumnw_readent(ucenumnw *op,ucenumnw_ent *ep,char *bp,int bl) noex {
	ucenumxxco<ucenumnw_ent>	ueo(op,mxx) ;
	return ueo.readent(ep,bp,bl) ;
}
/* end subroutine (ucenumnw_readent) */

int ucenumnw_reset(ucenumnw *op) noex {
	ucenumxxco<ucenumnw_ent>	ueo(op,mxx) ;
	return ueo.reset() ;
}
/* end subroutine (ucenumnw_reset) */


/* local subroutines */

int ucenumnw::readent(ucenumnw_ent *uap,char *uabuf,int ualen) noex {
	return ucenumnw_readent(this,uap,uabuf,ualen) ;
}

void ucenumnw::dtor() noex {
	if (cint rs = close ; rs < 0) {
	    ulogerror("ucenumnw",rs,"fini-close") ;
	}
}

int ucenumnw_op::operator () (cchar *fn) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case ucenumnwmem_open:
	        rs = ucenumnw_open(op,fn) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (ucenumnw_op::operator) */

ucenumnw_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case ucenumnwmem_reset:
	        rs = ucenumnw_reset(op) ;
	        break ;
	    case ucenumnwmem_close:
	        rs = ucenumnw_close(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (ucenumnw_co::operator) */



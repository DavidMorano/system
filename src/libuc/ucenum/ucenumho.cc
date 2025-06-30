/* ucenumho SUPPORT */
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
	ucenumho

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
#include	"ucenumho.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr uint		mxx = UCENUMHO_MAGIC ;
constexpr sysdbfiles	w = sysdbfile_ho ;


/* local subroutines */


/* exported variables */


/* exported subroutines */

int ucenumho_open(ucenumho *op,cchar *efname) noex {
	ucenumxxco<ucenumho_ent>	ueo(op,mxx) ;
	int		rs ;
	if (cchar *rp ; (rs = sysdbfn_get(w,efname,&rp)) >= 0) {
	    rs = ueo.open(rp) ;
	} /* end if (sysdbfn_get) */
	return rs ;
}
/* end if (ucenumho_open) */

int ucenumho_close(ucenumho *op) noex {
	ucenumxxco<ucenumho_ent>	ueo(op,mxx) ;
	return ueo.close() ;
} 
/* end subroutine (ucenumho_close) */

int ucenumho_readent(ucenumho *op,ucenumho_ent *ep,char *bp,int bl) noex {
	ucenumxxco<ucenumho_ent>	ueo(op,mxx) ;
	return ueo.readent(ep,bp,bl) ;
}
/* end subroutine (ucenumho_readent) */

int ucenumho_reset(ucenumho *op) noex {
	ucenumxxco<ucenumho_ent>	ueo(op,mxx) ;
	return ueo.reset() ;
}
/* end subroutine (ucenumho_reset) */


/* local subroutines */

int ucenumho::readent(ucenumho_ent *uap,char *uabuf,int ualen) noex {
	return ucenumho_readent(this,uap,uabuf,ualen) ;
}

void ucenumho::dtor() noex {
	if (cint rs = close ; rs < 0) {
	    ulogerror("ucenumho",rs,"fini-close") ;
	}
}

int ucenumho_op::operator () (cchar *fn) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case ucenumhomem_open:
	        rs = ucenumho_open(op,fn) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (ucenumho_op::operator) */

ucenumho_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case ucenumhomem_reset:
	        rs = ucenumho_reset(op) ;
	        break ;
	    case ucenumhomem_close:
	        rs = ucenumho_close(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (ucenumho_co::operator) */



/* ucenumua SUPPORT */
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
	ucenumua

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
#include	"ucenumua.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr uint		mxx = UCENUMUA_MAGIC ;
constexpr sysdbfiles	w = sysdbfile_ua ;


/* local subroutines */


/* exported variables */


/* exported subroutines */

int ucenumua_open(ucenumua *op,cchar *efname) noex {
	ucenumxxco<ucenumua_ent>	ueo(op,mxx) ;
	int		rs ;
	if (cchar *rp ; (rs = sysdbfn_get(w,efname,&rp)) >= 0) {
	    rs = ueo.open(rp) ;
	} /* end if (sysdbfn_get) */
	return rs ;
}
/* end if (ucenumua_open) */

int ucenumua_close(ucenumua *op) noex {
	ucenumxxco<ucenumua_ent>	ueo(op,mxx) ;
	return ueo.close() ;
} 
/* end subroutine (ucenumua_close) */

int ucenumua_readent(ucenumua *op,ucenumua_ent *ep,char *bp,int bl) noex {
	ucenumxxco<ucenumua_ent>	ueo(op,mxx) ;
	return ueo.readent(ep,bp,bl) ;
}
/* end subroutine (ucenumua_readent) */

int ucenumua_reset(ucenumua *op) noex {
	ucenumxxco<ucenumua_ent>	ueo(op,mxx) ;
	return ueo.reset() ;
}
/* end subroutine (ucenumua_reset) */


/* local subroutines */

int ucenumua::readent(ucenumua_ent *uap,char *uabuf,int ualen) noex {
	return ucenumua_readent(this,uap,uabuf,ualen) ;
}

void ucenumua::dtor() noex {
	if (cint rs = close ; rs < 0) {
	    ulogerror("ucenumua",rs,"fini-close") ;
	}
}

int ucenumua_op::operator () (cchar *fn) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case ucenumuamem_open:
	        rs = ucenumua_open(op,fn) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (ucenumua_op::operator) */

ucenumua_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case ucenumuamem_reset:
	        rs = ucenumua_reset(op) ;
	        break ;
	    case ucenumuamem_close:
	        rs = ucenumua_close(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (ucenumua_co::operator) */



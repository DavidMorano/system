/* ucenumgr SUPPORT */
/* encoding=ISO8859-1 */
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
	ucenumgr

	Description:
	We enumerate (reentrantly and thread safely) SYSDB entries
	from the system SYSDB-XX database.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<sysdbfname.h>
#include	<localmisc.h>

#include	"ucenumxx.h"
#include	"ucenumgr.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr uint		mxx = UCENUMGR_MAGIC ;
constexpr sysdbfiles	w = sysdbfile_gr ;


/* local subroutines */


/* exported variables */


/* exported subroutines */

int ucenumgr_open(ucenumgr *op,cchar *efname) noex {
	ucenumxxco<ucenumgr_ent>	ueo(op,mxx) ;
	int		rs ;
	if (cchar *rp{} ; (rs = sysdbfnameget(w,efname,&rp)) >= 0) {
	    rs = ueo.open(rp) ;
	} /* end if (sysdbfnameget) */
	return rs ;
}
/* end if (ucenumgr_open) */

int ucenumgr_close(ucenumgr *op) noex {
	ucenumxxco<ucenumgr_ent>	ueo(op,mxx) ;
	return ueo.close() ;
} 
/* end subroutine (ucenumgr_close) */

int ucenumgr_readent(ucenumgr *op,ucenumgr_ent *ep,char *bp,int bl) noex {
	ucenumxxco<ucenumgr_ent>	ueo(op,mxx) ;
	return ueo.readent(ep,bp,bl) ;
}
/* end subroutine (ucenumgr_readent) */

int ucenumgr_reset(ucenumgr *op) noex {
	ucenumxxco<ucenumgr_ent>	ueo(op,mxx) ;
	return ueo.reset() ;
}
/* end subroutine (ucenumgr_reset) */


/* local subroutines */

int ucenumgr::readent(ucenumgr_ent *uap,char *uabuf,int ualen) noex {
	return ucenumgr_readent(this,uap,uabuf,ualen) ;
}

void ucenumgr::dtor() noex {
	if (cint rs = close ; rs < 0) {
	    ulogerror("ucenumgr",rs,"fini-close") ;
	}
}

int ucenumgr_op::operator () (cchar *fn) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case ucenumgrmem_open:
	        rs = ucenumgr_open(op,fn) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (ucenumgr_op::operator) */

ucenumgr_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case ucenumgrmem_reset:
	        rs = ucenumgr_reset(op) ;
	        break ;
	    case ucenumgrmem_close:
	        rs = ucenumgr_close(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (ucenumgr_co::operator) */



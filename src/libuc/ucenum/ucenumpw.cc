/* ucenumpw SUPPORT */
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
	ucenumpw

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
#include	"ucenumpw.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr uint		mxx = UCENUMPW_MAGIC ;
constexpr sysdbfiles	w = sysdbfile_pw ;


/* local subroutines */


/* exported variables */


/* exported subroutines */

int ucenumpw_open(ucenumpw *op,cchar *efname) noex {
	ucenumxxco<ucenumpw_ent>	ueo(op,mxx) ;
	int		rs ;
	if (cchar *rp{} ; (rs = sysdbfnameget(w,efname,&rp)) >= 0) {
	    rs = ueo.open(rp) ;
	} /* end if (sysdbfnameget) */
	return rs ;
}
/* end if (ucenumpw_open) */

int ucenumpw_close(ucenumpw *op) noex {
	ucenumxxco<ucenumpw_ent>	ueo(op,mxx) ;
	return ueo.close() ;
} 
/* end subroutine (ucenumpw_close) */

int ucenumpw_readent(ucenumpw *op,ucenumpw_ent *ep,char *bp,int bl) noex {
	ucenumxxco<ucenumpw_ent>	ueo(op,mxx) ;
	return ueo.readent(ep,bp,bl) ;
}
/* end subroutine (ucenumpw_readent) */

int ucenumpw_reset(ucenumpw *op) noex {
	ucenumxxco<ucenumpw_ent>	ueo(op,mxx) ;
	return ueo.reset() ;
}
/* end subroutine (ucenumpw_reset) */


/* local subroutines */

int ucenumpw::readent(ucenumpw_ent *uap,char *uabuf,int ualen) noex {
	return ucenumpw_readent(this,uap,uabuf,ualen) ;
}

void ucenumpw::dtor() noex {
	if (cint rs = close ; rs < 0) {
	    ulogerror("ucenumpw",rs,"fini-close") ;
	}
}

int ucenumpw_op::operator () (cchar *fn) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case ucenumpwmem_open:
	        rs = ucenumpw_open(op,fn) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (ucenumpw_op::operator) */

ucenumpw_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case ucenumpwmem_reset:
	        rs = ucenumpw_reset(op) ;
	        break ;
	    case ucenumpwmem_close:
	        rs = ucenumpw_close(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (ucenumpw_co::operator) */



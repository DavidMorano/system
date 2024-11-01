/* ucenumgr SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* enumerate system SYSDB-XX entries */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was morphed from some previous one. I
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
#include	<sys/types.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
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



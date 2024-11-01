/* ucenumua SUPPORT */
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
	ucenumua

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
	if (cchar *rp{} ; (rs = sysdbfnameget(w,efname,&rp)) >= 0) {
	    rs = ueo.open(rp) ;
	} /* end if (sysdbfnameget) */
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



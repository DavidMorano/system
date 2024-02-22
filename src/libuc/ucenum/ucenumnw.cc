/* ucenumnw SUPPORT */
/* lang=C++20 */

/* enumerate system SYSDB-XX entries */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A�D� Morano
	This object module was morphed from some previous one. I
	do not remember what the previous one was.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	We enumerate (reentrantly and thread safely) SYSDB entries
	from the system SYSDB-XX database.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<sysdbfname.h>
#include	<localmisc.h>

#include	"ucenumxx.h"
#include	"ucenumnw.h"


/* local defines */


/* local namespaces */


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
	if (cchar *rp{} ; (rs = sysdbfnameget(w,efname,&rp)) >= 0) {
	    rs = ueo.open(rp) ;
	} /* end if (sysdbfnameget) */
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



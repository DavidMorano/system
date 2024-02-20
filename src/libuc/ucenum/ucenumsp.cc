/* ucenumsp SUPPORT */
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

	We enumerate (reentrantly and thread safely) SYSDB entries
	from the system SYSDB-XX database.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<sysdbfname.h>
#include	<localmisc.h>

#include	"ucenumxx.h"
#include	"ucenumsp.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr uint		mxx = UCENUMSP_MAGIC ;
constexpr sysdbfiles	w = sysdbfile_sp ;


/* local subroutines */


/* exported variables */


/* exported subroutines */

int ucenumsp_open(ucenumsp *op,cchar *efname) noex {
	ucenumxxco<ucenumsp_ent>	ueo(op,mxx) ;
	int		rs ;
	if (cchar *rp ; (rs = sysdbfnameget(w,efname,&rp)) >= 0) {
	    rs = ueo.open(rp) ;
	} /* end if (sysdbfnameget) */
	return rs ;
}
/* end if (ucenumsp_open) */

int ucenumsp_close(ucenumsp *op) noex {
	ucenumxxco<ucenumsp_ent>	ueo(op,mxx) ;
	return ueo.close() ;
} 
/* end subroutine (ucenumsp_close) */

int ucenumsp_readent(ucenumsp *op,ucenumsp_ent *ep,char *bp,int bl) noex {
	ucenumxxco<ucenumsp_ent>	ueo(op,mxx) ;
	return ueo.readent(ep,bp,bl) ;
}
/* end subroutine (ucenumsp_readent) */

int ucenumsp_reset(ucenumsp *op) noex {
	ucenumxxco<ucenumsp_ent>	ueo(op,mxx) ;
	return ueo.reset() ;
}
/* end subroutine (ucenumsp_reset) */


/* local subroutines */


